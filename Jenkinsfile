@Library('xmos_jenkins_shared_library@v0.30.0') _

getApproval()

// Get XCommon CMake and log a record of the git commit
def get_xcommon_cmake() {
  sh "git clone -b develop git@github.com:xmos/xcommon_cmake"
  sh "git -C xcommon_cmake rev-parse HEAD"
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
    PIP_VERSION = "24.0"
    PYTHON_VERSION = "3.11"
    XMOSDOC_VERSION = "v4.0"
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
            get_xcommon_cmake()

            dir("${REPO}") {
              checkout scm
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
            catchError(buildResult: 'SUCCESS', stageResult: 'FAILURE') {
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
                      sh "python -m pytest -k \"lib\" --junitxml=junit_lib.xml"
                      junit "junit_lib.xml"
                    } // withEnv
                  } // with Venv
                } // tools
              } // dir
            } // catch error
          } //step
        }  // Library checks
        stage('Build examples XCCM') {
          steps {
            withTools(params.TOOLS_VERSION) {
              withEnv(["XMOS_CMAKE_PATH=${WORKSPACE}/xcommon_cmake"]) {
                dir("${REPO}") {
                  withCredentials([file(credentialsId: 'DSPCAWE_8.D.1.1', variable: 'DSPC_AWE_LIB')]) {
                    sh "cp ${DSPC_AWE_LIB} lib_awe/lib/xs3a" // Bring AWE library in
                    script {
                      // Build all apps in the examples directory
                      def apps = sh(script: "ls -d app_*", returnStdout: true).trim()
                      for(String app : apps.split()) {
                        dir("${app}") {
                          sh "cmake  -G \"Unix Makefiles\" -B build"
                          sh "xmake -C build -j"
                        } // dir
                      } // for loop
                    } // script
                  } // credentials
                } // dir
                archiveArtifacts artifacts: "${REPO}/**/*.xe", allowEmptyArchive: false
                stash name: "xe_files", includes: "${REPO}/**/*.xe"
              } // withEnv
            } // withTools
          } // steps
        }  // Build examples XCCM
        stage('Build examples xmake') {
          steps {
            withTools(params.TOOLS_VERSION) {
              dir("${REPO}") {
                script {
                  // Build all apps in the examples directory
                  def apps = sh(script: "ls -d app_*", returnStdout: true).trim()
                  for(String app : apps.split()) {
                    dir("${app}") {
                      // Disable xmake build for now until fixed
                      // sh "xmake -j"
                    }
                  } // for loop
                } // script
              } // dir
            // archiveArtifacts artifacts: "${REPO}/**/bin/*.xe", allowEmptyArchive: false
            } // withTools
          } // steps
        }  // Build examples
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
            dir("${REPO}") {
              checkout scm
              sh "git clone git@github.com:xmos/sw_usb_audio"
              createVenv()
              withVenv {
                sh 'pip install -r requirements.txt'
                sh 'pip install -r ../../sw_usb_audio/requirements.txt'
              }
            }
            dir("sw_audio_analyzer") {
              copyArtifacts filter: '**/*.xe', fingerprintArtifacts: true, projectName: 'xmos-int/sw_audio_analyzer/master', selector: lastSuccessful()
              copyArtifacts filter: 'host_xscope_controller/bin_macos/xscope_controller', fingerprintArtifacts: true, projectName: 'xmos-int/sw_audio_analyzer/master', selector: lastSuccessful()
            }
            println "Stage running on ${env.NODE_NAME}"
            dir("${REPO}/tests") {
              withEnv(["XMOS_CMAKE_PATH=${WORKSPACE}/xcommon_cmake"]) {
                withVenv {
                  withTools(params.TOOLS_VERSION) {
                    dir("tools") {
                      // Build test support application
                      // dir("volcontrol") {
                      //   sh 'cmake -B build'
                      //   sh 'make -C build'
                      // }
                      copyArtifacts filter: 'bin-macos-x86/xsig', fingerprintArtifacts: true, projectName: 'xmos-int/xsig/master', flatten: true, selector: lastSuccessful()
                      copyArtifacts filter: 'OSX/x86/xmos_mixer', fingerprintArtifacts: true, projectName: 'XMOS/lib_xua/develop', flatten: true, selector: lastSuccessful()
                    }

                    unstash "xe_files"
                    sh "tree .."
                    sh "xrun -l"
                  } // Tools
                } // Venv
              } // XCCM
            } // dir
          } // steps
          post {
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
            sh "git clone git@github.com:xmos/sw_usb_audio"
            dir("${REPO}") {
              checkout scm
              createVenv()
              withVenv {
                sh 'pip install -r requirements.txt'
                sh 'pip install -r ../../sw_usb_audio/requirements.txt'
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
                    sh "python -m pytest -k \"not lib and not audio\" --junitxml=junit_main.xml"
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
            dir("${REPO}") {
              checkout scm
            
              sh "docker pull ghcr.io/xmos/xmosdoc:$XMOSDOC_VERSION"
              sh """docker run -u "\$(id -u):\$(id -g)" \
                      --rm \
                      -v ${WORKSPACE}/${REPO}:/build \
                      ghcr.io/xmos/xmosdoc:$XMOSDOC_VERSION -v"""
              // Zip and archive doc files
              zip dir: "doc/_build/html/", zipFile: "lib_awe_docs_html.zip"
              archiveArtifacts artifacts: "lib_awe_docs_html.zip"
              archiveArtifacts artifacts: "doc/_build/**/*.pdf", allowEmptyArchive: true
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