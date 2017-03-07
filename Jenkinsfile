pipeline {
    agent { docker 'apiaryio/emcc' }
    stages {
        stage('build') {
            steps {
                sh 'emcc --version'
            }
        }
    }
}
