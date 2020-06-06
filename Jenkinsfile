
pipeline {
    agent any

    stages {
        stage('Build') {
            steps {
                sh -c "meson build"
                sh -c "ninja -C build"
            }
        }
        stage('Test') {
            steps {
                sh -c "ninja -C build test"
            }
        }
    }
}
