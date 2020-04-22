# Dohyeon 의 C++ 환경에서의 ModbusTcp

- Server(Slave), Client(Master) 의 통신 프로토콜로 Register Input , Write 등을 기반으로 개발
- Multi Thread 를 사용하였으며, std::thread 이다.


# 작성한 코드의 문제점
 
- Client 즉 Master 의 접근을 너무 정적으로 만들었다. 콘솔을 하나더 만들어 접근하게 했으면 좀 더 실용성이 있었을 것이다.
- Server 즉 Slave 가 무한정 기다리게끔 하는것이 더 효율이 있다고 보여진다. 작성 코드에서는 Thread 를 통해 기다린다.
- connect시 예외가 좀 아쉽다. socket errer 시 에만 예외가 되어있어는데, connect에서 time delay 가 발생되는 예외 추가가 필요해 보인다.
- 비동기 방식으로 구현하는것이 퍼포먼스가 더 좋아보여진다. 속도측면에서 
- 실무에서는 10 m/s 단위로 통신이 진행 될텐데, 그럴경우 위에서 작성한 server 에서의 for 문 count는 문제가 될 것이다. 또한 recv, send buffer를   heap memory 영역으로 잡았는데 그럴경우도 문제가 될것이다.
- 동적 바인딩을 vector 로 구현했는데 list 로 하는게 더 나을수 있겠다.. 물론 위의 상황에서는 문제가 전혀 안되지만 매우 규모가 커질 경우..
