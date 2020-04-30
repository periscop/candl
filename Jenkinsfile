pipeline {
  agent none
  stages{
    stage('Candl'){
      matrix{
        agent{ label "${OS}" }
        axes{
          axis{
            name 'OS'
            values 'Ubuntu', 'macOS', 'fedora', 'Debian'
          }
          axis{
            name 'BuildSystem'
            values 'GNU Autotools'//, 'CMake'
          }
        }
        stages{
          stage('Tools'){
            steps{
              script{
                if(env.OS == 'Ubuntu')
                  sh 'sudo apt install flex bison doxygen texinfo -y'
                if(env.OS == 'macOS')
                  sh 'brew install automake libtool'
                if(env.OS == 'CentOS')
                  sh 'sudo yum install gmp-devel flex bison doxygen texinfo -y'
                if(env.OS == 'fedora')
                  sh 'sudo dnf install gmp-devel flex doxygen texinfo -y'
                if(env.OS == 'Debian')
                  sh 'sudo apt install autoconf libtool libgmp-dev make flex bison doxygen texinfo -y'
              }
            }
          }
          stage('Build'){
            steps{
              script{
                if(env.BuildSystem == 'GNU Autotools')
                  sh './get_submodules.sh && ./autogen.sh && ./configure --with-osl=bundled --with-piplib=bundled && make -j'
                if(env.BuildSystem == 'CMake')
                  sh 'mkdir build && cd build && cmake .. && cmake --build'
              }
            }
          }
          stage('Test'){
            steps{
              script {
                if(env.BuildSystem == 'GNU Autotools')
                  sh 'make check'
                if(env.BuildSystem == 'CMake')
                  sh 'cd build && cmake --build . --target check'
              }
            }
          }
        }
      }
    }
  }
}

