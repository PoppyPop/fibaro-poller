CREATE DATABASE IF NOT EXISTS `temperatures`;

USE `temperatures`;

CREATE TABLE IF NOT EXISTS `temperatures` (
  `cle` bigint(10) NOT NULL AUTO_INCREMENT,
  `datecapture` datetime NOT NULL,
  
  `piece` int(5) ,
  `id` int(5) ,
  `nom` varchar(255),
  `temp` float(3,2),
  
  PRIMARY KEY (`cle`)
);

CREATE UNIQUE INDEX IDX_TEMP_DATECAPTURE ON temperatures (datecapture, id);
