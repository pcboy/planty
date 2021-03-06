all:
	docker-compose up -d influx

influx:
	docker-compose exec influx influx -username admin -password admin -execute "CREATE USER planty WITH PASSWORD '$(password)' WITH ALL PRIVILEGES"
	docker-compose exec influx influx -username planty -password '$(password)' -execute "CREATE DATABASE planty"
