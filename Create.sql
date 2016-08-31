set DATEFORMAT dmy

create table main(
quote smallint NOT NULL,
time time NOT NULL,
timeframe smallint NOT NULL,
maBuy tinyint,
maSell tinyint,
tiBuy tinyint,
tiSell tinyint,
primary key (quote, time, timeframe)
)

use Exchange
CREATE LOGIN user1
WITH PASSWORD = 'user'
CREATE USER user1 FOR LOGIN user1

grant insert on main to user1