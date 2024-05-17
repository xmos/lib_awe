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
        withTools(params.TOOLS_VERSION) {
          // creation of tools_released and REPO environment variable are workarounds
          // to allow xcoreLibraryChecks to run without a viewfile-based sandbox
          dir("tools_released") {
            sh "echo ${params.TOOLS_VERSION} > REQUIRED_TOOLS_VERSION"
          }
          println "Library checks not active yet"
          // withEnv(["REPO=${REPO}"]) {
          //   xcoreLibraryChecks("${REPO}", false)
          // }
        }
      }
    }  // Library checks
    stage('Build examples XCCM') {
      steps {
        withTools(params.TOOLS_VERSION) {
          withEnv(["XMOS_CMAKE_PATH=${WORKSPACE}/xcommon_cmake"]) {
            dir("${REPO}") {
              script {
                // Build all apps in the examples directory
                def apps = sh(script: "ls -d app_*", returnStdout: true).trim()
                for(String app : apps.split()) {
                  dir("${app}") {
                    sh "cmake  -G \"Unix Makefiles\" -B build"
                    sh "xmake -C build -j"
                  }
                } // for loop
              } // script
            } // dir
            archiveArtifacts artifacts: "${REPO}/**/bin/*.xe", allowEmptyArchive: false
          } // withEnv
        } // withTools
      } // steps
    }  // Build examples
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