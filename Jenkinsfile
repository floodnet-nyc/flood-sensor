pipeline {
    // agent {
    //     docker {
    //         image 'praneethsvch/fn-docker:v0.3'
    //         args '-v /dev/tty.usbmodem8402:/dev/tty.usbmodem8402'
    //     }
    // }

    agent any
    
    stages {
        stage('Checkout') {
            steps {
                checkout([$class: 'GitSCM',
                          branches: [[name: 'Develop']], 
                          userRemoteConfigs: [[url: 'https://github.com/floodnet-nyc/flood-sensor.git']]])  
            }
        }
        
        stage('Build') {
            steps {
                sh """
                    if [ ! -d "firmware/lora-e5/build" ]; then
                        echo "Build folder does not exist. Creating..."
                        mkdir -p "firmware/lora-e5/build"
                    fi
                    cd "firmware/lora-e5/build"
                    cmake ..
                    make
                """
            }
        }
        
        // stage('Test') {
        //     steps {
        //         // Run your automated tests
        //         // This can include unit tests, integration tests, etc.
        //         // Example for running tests with Unity framework:
        //         // sh 'make test'
        //     }
        // }
        
        stage('Deploy') {
            steps {
                // Transfer the compiled firmware to the the lora-e5
                sh """
                    openocd -f /usr/share/openocd/scripts/interface/stlink.cfg   \
                        -f /usr/share/openocd/scripts/target/stm32wlx.cfg            \
                        -c init -c "reset halt"                                     \
                        -c "flash write_image erase lora-e5.bin 0x08000000"         \
                        -c "verify reset exit"
                """ 
            }
        }
    }
}
