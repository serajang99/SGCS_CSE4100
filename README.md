# SGCS_CSE4100
## 시스템프로그래밍 (CSE4100), 2020-1
## SIC/XE 머신 구현
### Project 1
이 프로그램은 앞으로 구현하게 될 SIC/XE 머신을 구현하기 위한 전 단계로서 어셈블러, 링크, 로더들을 실행하게 될 셸(shell)과 컴파일을 통해서 만들어진 object 코드가 적재 되고 실행될 메모리공간과 mnemonic (ADD, COMP, FLOAT, etc ...)을 opcode 값으로 변환하는 OPCODE 테이블과 관련 명령어들을 구현하는 프로그램입니다.

### Project 2
프로젝트1에서 구현한 셀(shell)에 assemble 기능을 추가하는 프로그램. SIC/XE의 assembly program source 파일을 입력 받아서 object파일을 생성하고, 어셈블리 과정 중 생성된 symbol table과 결과물인 object 파일을 볼 수 있는 기능을 제공해야 함. 교재의 2.2까지 설명된 SIC/XE 어셈블러의 기능을 구현함을 원칙으로 한다.

### Project 3
프로젝트 1, 2 에서 구현한 셀(shell)에 linking과 loading 기능을 추가하는 프로그램이다.
프로젝트 2 에서 구현된 assemble 명령을 통해서 생성된 object 파일을 link시켜 메모리에 올리는 일을 수행한다.


## 머신러닝 모델 구현
### Project 4
MNIST 예제를 바탕으로 Google Colab에서 Keras를 이용하여 이미지 분류를 하는 머신 러닝 모델을 개발한다.
CIFAR-10이라는 dataset을 이용하여 이미지 분류 성능을 측정하고 직접 모델의 파라미터들을 바꿔가며 테스트 셋에 대한 정확도 80% 이상을 얻는 것을 목표로 한다.
