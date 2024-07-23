@Library('xmos_jenkins_shared_library@v0.30.0') _

getApproval()

// Get XCommon CMake and log a record of the git commit
def get_xcommon_cmake() {
  sh 'if [ ! -d "xcommon_cmake"]; then git clone -b v1.2.0 git@github.com:xmos/xcommon_cmake; else echo "xcommon_cmake already cloned"; fi'
  sh 'git -C xcommon_cmake rev-parse HEAD'
}

pipeline {
  agent none

  options {
    buildDiscarder(xmosDiscardBuildSettings())
    skipDefaultCheckout()
  }
  parameters {
    string(
      name: 'TOOLS_VERSION',
      defaultValue: '15.2.1',
      description: 'The XTC tools version'
    )
  }
  environment {
    REPO = 'lib_awe'
    EXAMPLE = 'an02016'
    PIP_VERSION = "24.0"
    PYTHON_VERSION = "3.11"
    XMOSDOC_VERSION = "v5.4"
    XTAGCTL_VERSION = "v2.0.0"
  }
  stages {
    stage('Linux stages') {
      agent {
        label 'x86_64 && linux'
      }
      stages{
        stage('Get sandbox') {
          steps {
            println "Stage running on: ${env.NODE_NAME}"

            sh "git clone -b v1.2.1 git@github.com:xmos/infr_scripts_py"
            sh "git clone -b v1.5.0 git@github.com:xmos/infr_apps"
            sh "git clone git@github.com:xmosnotes/${EXAMPLE}.git"
            get_xcommon_cmake()

            dir("${REPO}") {
              checkout scm
              sh "git submodule update --init"
              createVenv()
              withVenv {
                sh "pip install -e ${WORKSPACE}/infr_scripts_py"
                sh "pip install -e ${WORKSPACE}/infr_apps"
                sh 'pip install -r requirements.txt'
              }
            } // dir
          } // steps
        } // Get sandbox

        stage('Library checks') {
          steps {
            dir("${REPO}") {
              withTools(params.TOOLS_VERSION) {
                withVenv {
                  // creation of tools_released and REPO environment variable are workarounds
                  // to allow xcoreLibraryChecks to run without a viewfile-based sandbox
                  dir("tools_released") {
                    sh "echo ${params.TOOLS_VERSION} > REQUIRED_TOOLS_VERSION"
                  }
                  withEnv(["REPO=${REPO}"]) {
                    xcoreLibraryChecks("${REPO}", false)
                    // Need to run this test on the repo source only before we do a build and grab the .a
                    sh "python -m pytest -m lib --junitxml=junit_lib.xml"
                    junit "junit_lib.xml"
                  } // withEnv
                } // with Venv
              } // tools
            } // dir
          } //step
        }  // Library checks
        stage('Build examples xcommon_cmake') {
          steps {
            withTools(params.TOOLS_VERSION) {
              withEnv(["XMOS_CMAKE_PATH=${WORKSPACE}/xcommon_cmake"]) {
                dir("${REPO}") {
                  withCredentials([file(credentialsId: 'DSPCAWE_8.D.1.1', variable: 'DSPC_AWE_LIB')]) {
                    sh "cp ${DSPC_AWE_LIB} lib_awe/lib/xs3a" // Bring AWE library in
                  }
                }
                dir("${EXAMPLE}") {
                  // Build all example apps
                  sh "cmake  -G \"Unix Makefiles\" -B build"
                  archiveArtifacts artifacts: "build/manifest.txt", allowEmptyArchive: false
                  sh "xmake -C build -j"
                  archiveArtifacts artifacts: "**/*.xe", allowEmptyArchive: false
                  stash name: "xe_files", includes: "**/*.xe"
                } // dir
              } // withEnv
            } // withTools
          } // steps
        }  // Build examples XCCM
      } // stages
      post {
        cleanup {
          xcoreCleanSandbox()
        }
      }
    } // on linux
    stage('Tests and docs') {
      parallel {
        stage('HW tests') {
          agent {
            label 'usb_audio && macos && arm64 && xcore.ai-mcab'
          }
          steps {
            println "Stage running on ${env.NODE_NAME}"
            sh "git clone git@github0.xmos.com:xmos-int/xtagctl"
            sh "git -C xtagctl checkout ${env.XTAGCTL_VERSION}"

            dir("sw_audio_analyzer") {
              copyArtifacts filter: '**/*.xe', fingerprintArtifacts: true, projectName: 'xmos-int/sw_audio_analyzer/master', selector: lastSuccessful()
              copyArtifacts filter: 'host_xscope_controller/bin_macos/xscope_controller', fingerprintArtifacts: true, projectName: 'xmos-int/sw_audio_analyzer/master', selector: lastSuccessful()
            }

            dir("${REPO}") {
              checkout scm
              sh "git submodule update --init"
              createVenv()
              withVenv {
                sh 'pip install -r requirements.txt'
              }
            }

            dir("${EXAMPLE}") {
              unstash "xe_files"
            }

            get_xcommon_cmake()

            dir("${REPO}/tests") {
              dir("hardware_test_tools/xsig") {
                copyArtifacts filter: 'bin-macos-arm/xsig', fingerprintArtifacts: true, projectName: 'xmos-int/xsig/master', flatten: true, selector: lastSuccessful()
              }

              withEnv(["XMOS_CMAKE_PATH=${WORKSPACE}/xcommon_cmake"]) {
                withVenv {
                  withTools(params.TOOLS_VERSION) {
                    sh "pip install -e ${WORKSPACE}/xtagctl"

                    // Get pre-built application example XEs
                    unstash "xe_files"

                    // Build test XEs
                    withTools(params.TOOLS_VERSION) {
                      dir("test_ffs_rpc"){
                        sh "cmake -G \"Unix Makefiles\" -B build"
                        sh "xmake -C build -j"
                      }
                      dir("test_ffs_awb_device"){
                        sh "cmake -G \"Unix Makefiles\" -B build"
                        sh "xmake -C build -j"
                      }
                    }

                    withXTAG(["usb_audio_mc_xcai_dut", "usb_audio_mc_xcai_harness"]) { xtagIds ->
                      sh "pytest -v -m hw --junitxml=pytest_result_hw.xml -o xtag_dut=${xtagIds[0]} -o xtag_harness=${xtagIds[1]}"
                    }
                  } // Tools
                } // Venv
              } // XCCM
            } // dir
          } // steps
          post {
            always {
              junit "${REPO}/tests/pytest_result_hw.xml"
            }
            cleanup {
              xcoreCleanSandbox()
            }
          }
        } // HW tests
        stage('Sim Tests') {
          agent {
            label 'x86_64 && linux'
          }
          steps {
            println "Stage running on ${env.NODE_NAME}"
            dir("${REPO}") {
              checkout scm
              sh "git submodule update --init"
              createVenv()
              withVenv {
                sh 'pip install -r requirements.txt'
              }
              withCredentials([file(credentialsId: 'DSPCAWE_8.D.1.1', variable: 'DSPC_AWE_LIB')]) {
                sh "cp ${DSPC_AWE_LIB} lib_awe/lib/xs3a" // Bring AWE library in
              }
            }
            get_xcommon_cmake()

            dir("${REPO}/tests") {
              withEnv(["XMOS_CMAKE_PATH=${WORKSPACE}/xcommon_cmake"]) {
                withVenv {
                  withTools(params.TOOLS_VERSION) {
                    dir("test_basic"){
                      sh "cmake -G \"Unix Makefiles\" -B build"
                      sh "xmake -C build -j"
                    }
                    dir("test_xawe_if"){
                      sh "cmake -G \"Unix Makefiles\" -B build"
                      sh "xmake -C build -j"
                    }
                    sh "python -m pytest -v -m sim --junitxml=junit_main.xml"
                    junit "junit_main.xml"
                  } // withTools
                } // withVenv
              } // withEnv
            } // dir
          } // steps
          post {
            cleanup {
              xcoreCleanSandbox()
            }
          }
        } // stage
        stage('Documentation') {
          agent {
            label 'docker'
          }
          steps {
            println "Stage running on ${env.NODE_NAME}"
            // We are going to build the example doc too for convinience
            sh "git clone git@github.com:xmosnotes/${EXAMPLE}.git"

            dir("${REPO}") {
              checkout scm
              sh "git submodule update --init"

              sh "docker pull ghcr.io/xmos/xmosdoc:$XMOSDOC_VERSION"

              // Build lib docs
              sh """docker run -u "\$(id -u):\$(id -g)" \
                      --rm \
                      -v ${WORKSPACE}/${REPO}:/build \
                      ghcr.io/xmos/xmosdoc:$XMOSDOC_VERSION -v"""
              // Zip and archive doc files
              zip dir: "doc/_build/html/", zipFile: "lib_awe_docs_html.zip"
              archiveArtifacts artifacts: "lib_awe_docs_html.zip"
              sh "cp doc/_build/pdf/*.pdf ." // Will archive at end of stage along with example

              // Build example docs
              sh """docker run -u "\$(id -u):\$(id -g)" \
                      --rm \
                      -v ${WORKSPACE}/${EXAMPLE}:/build \
                      ghcr.io/xmos/xmosdoc:$XMOSDOC_VERSION -v"""
              // Zip and archive doc files
              zip dir: "../${EXAMPLE}/doc/_build/html/", zipFile: "${EXAMPLE}_html.zip"
              archiveArtifacts artifacts: "${EXAMPLE}_html.zip"
              sh "cp ../${EXAMPLE}/doc/_build/pdf/*.pdf ." // archiveArtifacts doesn't like going up a dir
              archiveArtifacts artifacts: "*.pdf", allowEmptyArchive: true
            }
          } // steps
          post {
            cleanup {
              xcoreCleanSandbox()
            }
          }
        } // Build documentation
      } // par
    } // Test and docs
  } // Stages
}
