if [docker network ls | grep "shiro"]; then
    docker network create shiro-network
fi