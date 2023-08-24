CREATE DATABASE fluffycoin;
CREATE USER fluffy WITH ENCRYPTED PASSWORD 'fluffy';
\c fluffycoin
GRANT CREATE ON SCHEMA public TO fluffy;
