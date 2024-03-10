pipeline {
    agent any    

    triggers { cron('0 0 * * 0') }

    stages {
        stage('Update submodules') {
            steps {
              sh 'git submodule init'
              sh 'git submodule update'
            }
        }

        stage('Build Frontend') {
            steps {
              sh 'cd frontend && docker build -t mse-frontend .'
              sh 'cd frontend && docker tag mse-frontend localhost:5000/mse-frontend'
            }
        }

        stage('Build Backend') {
            steps {
              sh 'docker build -t mse-backend .'
              sh 'docker tag mse-backend localhost:5000/mse-backend'
            }
        }

        stage('Push images') {
          steps {
            sh 'docker push localhost:5000/mse-frontend'
            sh 'docker push localhost:5000/mse-backend'
          }
        }

        stage('Package Frontend') {
          steps {
            sh '/var/lib/jenkins/go/bin/helmVersioner charts/mse-frontend/Chart.yaml'
            sh 'helm install mse-frontend charts/mse-frontend || true'
            sh 'helm upgrade mse-frontend charts/mse-frontend'
          }
        }

        stage('Package Backend') {
          steps {
            sh '/var/lib/jenkins/go/bin/helmVersioner charts/mse-backend/Chart.yaml'
            sh 'helm install mse-backend charts/mse-backend || true'
            sh 'helm upgrade mse-backend charts/mse-backend'
          }
        }
    }
}
