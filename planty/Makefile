influxdb:
	docker-compose up influx -d
	docker-compose exec influx influx -username admin -password admin -execute "CREATE USER planty WITH PASSWORD '$(password)' WITH ALL PRIVILEGES"
	docker-compose exec influx influx -username planty -password 'test' -execute "CREATE DATABASE planty"
