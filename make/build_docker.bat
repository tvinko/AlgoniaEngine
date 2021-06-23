set algov=1.0.0
set netcorev=netcore31
set pyv=py38

docker login --username=tvinko
docker build --tag algonia:%algov%-%pyv%-%netcorev% .
docker tag algonia:%algov%-%pyv%-%netcorev% tvinko/algonia:%algov%-%pyv%-%netcorev%
docker push tvinko/algonia:%algov%-%pyv%-%netcorev%


