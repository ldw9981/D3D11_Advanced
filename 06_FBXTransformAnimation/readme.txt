목표

블렌더의 Scenegraph 에서 Transform 정보를 갖는 오브젝트는 모두 aiNode가 될수 있다.
이 오브젝트들은 부모자식 관계를 연결 시켜 트리 구조의 형태를 이루게 되며
부모의 WorldTransform과 자신의 LocalTransform을 곱하여 자신의 WorldTransform을 계산한다.
이 LocalTransform은 aiAnimation의 aiNodeAnim에서 애니메이션이 진행되는 시간에 해당되는 행렬을 구할수있다.
