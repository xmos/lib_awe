@Library('xmos_jenkins_shared_library@v0.30.0') _

getApproval()

// Get XCommon CMake and log a record of the git commit
def get_xcommon_cmake() {
  sh "git clone -b develop git@github.com:xmos/xcommon_cmake"
  sh "git -C xcommon_cmake rev-parse HEAD"
}

pipeline {
  agent {
    label 'x86_64 && linux'
  }
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
    PYTHON_VERSION = "3.12.1"
  }
  stages {
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
          }
        }
      }
    }  // Get sandbox
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
                sh "python -m pytest -k \"lib\" --junitxml=junit_lib.xml"
                junit "junit_lib.xml"
              } // withEnv
            } // with Venv
          } // tools
        } // dir
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
            archiveArtifacts artifacts: "${REPO}/**/bin/*.xe", allowEmptyArchive: false
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
    stage('Tests') {
      steps {
        dir("${REPO}/tests") {
          withEnv(["XMOS_CMAKE_PATH=${WORKSPACE}/xcommon_cmake"]) {
            withVenv {
              withTools(params.TOOLS_VERSION) {
                dir("test_basic"){
                  sh "cmake  -G \"Unix Makefiles\" -B build"
                  sh "xmake -C build -j"
                }
                sh "python -m pytest -k \"not lib\" --junitxml=junit_main.xml"
                junit "junit_main.xml"
              } // withTools
            } // withVenv
          } // withEnv
        } // dir
      } // steps
    }  // Library checks
    stage('Build documentation') {
      steps {
        sh "echo hello"
      }
    }  // Build documentation
  } // Stages
  post {
    cleanup {
      xcoreCleanSandbox()
    }
  }
}