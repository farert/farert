BEGIN TRANSACTION;
CREATE TABLE t_jct (
	line_id 	integer not null references t_line(rowid),
	station_id 	integer not null references t_station(rowid),
	
	primary key (line_id, station_id)
);

COMMIT;
