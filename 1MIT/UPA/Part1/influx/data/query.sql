USE upa
precision RFC3339
-- first query
SELECT * FROM "co_8h" WHERE time > '2023-04-01' AND time <= '2023-05-01' AND code = 'BBMLA';
SELECT * FROM "no2_1h" WHERE time > '2023-04-01' AND time <= '2023-05-01' AND code = 'BBNYA';
SELECT * FROM "o3_1h" WHERE time > '2023-04-01' AND time <= '2023-05-01' AND code = 'BBNYA';
SELECT * FROM "pm10_1h" WHERE time > '2023-04-01' AND time <= '2023-05-01' AND code = 'BBNYA';
SELECT * FROM "pm10_24h" WHERE time > '2023-04-01' AND time <= '2023-05-01' AND code = 'BBNYA';
SELECT * FROM "pm2_5_1h" WHERE time > '2023-04-01' AND time <= '2023-05-01' AND code = 'BBNYA';
SELECT * FROM "so2_1h" WHERE time > '2023-04-01' AND time <= '2023-05-01' AND code = 'BBNYA';

-- second query
SELECT mean(value) FROM "co_8h" WHERE time > '2023-08-01' AND time <= '2023-09-01';
SELECT mean(value) FROM "no2_1h" WHERE time > '2023-08-01' AND time <= '2023-09-01';
SELECT mean(value) FROM "o3_1h" WHERE time > '2023-08-01' AND time <= '2023-09-01';
SELECT mean(value) FROM "pm10_1h" WHERE time > '2023-08-01' AND time <= '2023-09-01';
SELECT mean(value) FROM "pm10_24h" WHERE time > '2023-08-01' AND time <= '2023-09-01';
SELECT mean(value) FROM "pm2_5_1h" WHERE time > '2023-08-01' AND time <= '2023-09-01';
SELECT mean(value) FROM "so2_1h" WHERE time > '2023-08-01' AND time <= '2023-09-01';

