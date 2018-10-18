-- MySQL dump 10.13  Distrib 5.7.23, for Linux (x86_64)
--
-- Host: 207.148.92.82    Database: netxms_db
-- ------------------------------------------------------
-- Server version	5.7.23-0ubuntu0.16.04.1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `cluster_info`
--

DROP TABLE IF EXISTS `cluster_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cluster_info` (
  `cluster_id` char(45) COLLATE utf8_unicode_ci NOT NULL,
  `cluster_type` int(11) NOT NULL,
  `cluster_name` varchar(45) CHARACTER SET utf8 DEFAULT NULL,
  `ip_address` varchar(45) COLLATE utf8_unicode_ci DEFAULT NULL,
  `port` int(5) DEFAULT NULL,
  `user_name` varchar(45) COLLATE utf8_unicode_ci DEFAULT NULL,
  `password` varchar(45) COLLATE utf8_unicode_ci DEFAULT NULL,
  PRIMARY KEY (`cluster_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `google_account`
--

DROP TABLE IF EXISTS `google_account`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `google_account` (
  `user_name` char(150) COLLATE utf8_unicode_ci NOT NULL,
  `api` varchar(150) COLLATE utf8_unicode_ci NOT NULL,
  `client_secret` varchar(150) COLLATE utf8_unicode_ci NOT NULL,
  `client_id` varchar(150) COLLATE utf8_unicode_ci NOT NULL,
  `account_type` int(11) NOT NULL,
  `app_name` varchar(45) CHARACTER SET utf8 DEFAULT NULL,
  PRIMARY KEY (`user_name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `home_channel`
--

DROP TABLE IF EXISTS `home_channel`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `home_channel` (
  `channel_id` char(30) COLLATE utf8_unicode_ci NOT NULL,
  `channel_name` varchar(45) CHARACTER SET utf8 DEFAULT NULL,
  `google_account_user_name` char(150) COLLATE utf8_unicode_ci NOT NULL,
  `video_number` int(11) DEFAULT NULL,
  `view_number` int(11) DEFAULT NULL,
  `subscriber` int(11) DEFAULT NULL,
  `date_created` int(11) DEFAULT NULL,
  `status` int(11) DEFAULT NULL,
  `note` varchar(250) COLLATE utf8_unicode_ci DEFAULT NULL,
  PRIMARY KEY (`channel_id`),
  KEY `fk_home_channel_google_account1_idx` (`google_account_user_name`),
  CONSTRAINT `fk_home_channel_google_account1` FOREIGN KEY (`google_account_user_name`) REFERENCES `google_account` (`user_name`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `mapping_config`
--

DROP TABLE IF EXISTS `mapping_config`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `mapping_config` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `render_command` varchar(10000) COLLATE utf8_unicode_ci DEFAULT NULL,
  `title_template` varchar(500) CHARACTER SET utf8 DEFAULT NULL,
  `desc_template` varchar(2000) CHARACTER SET utf8 DEFAULT NULL,
  `tag_template` varchar(500) CHARACTER SET utf8 DEFAULT NULL,
  `enable_title` int(11) DEFAULT NULL,
  `enable_desc` int(11) DEFAULT NULL,
  `enable_tag` int(11) DEFAULT NULL,
  `mapping_list_id` int(11) NOT NULL,
  PRIMARY KEY (`id`,`mapping_list_id`),
  KEY `fk_mapping_config_mapping_list1_idx` (`mapping_list_id`),
  CONSTRAINT `fk_mapping_config_mapping_list1` FOREIGN KEY (`mapping_list_id`) REFERENCES `mapping_list` (`id`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB AUTO_INCREMENT=26 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `mapping_list`
--

DROP TABLE IF EXISTS `mapping_list`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `mapping_list` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `mapping_type` int(11) NOT NULL,
  `status_sync` int(11) NOT NULL,
  `time_interval_sync` int(11) DEFAULT NULL,
  `home_channel_id` char(30) COLLATE utf8_unicode_ci NOT NULL,
  `monitor_content` varchar(500) COLLATE utf8_unicode_ci DEFAULT NULL,
  `download_cluster` char(45) COLLATE utf8_unicode_ci DEFAULT NULL,
  `render_cluster` char(45) COLLATE utf8_unicode_ci DEFAULT NULL,
  `upload_cluster` char(45) COLLATE utf8_unicode_ci DEFAULT NULL,
  `last_sync_time` int(11) DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `fk_mapping_list_home_channel1_idx` (`home_channel_id`),
  CONSTRAINT `fk_mapping_list_home_channel1` FOREIGN KEY (`home_channel_id`) REFERENCES `home_channel` (`channel_id`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB AUTO_INCREMENT=26 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `monitor_channel`
--

DROP TABLE IF EXISTS `monitor_channel`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `monitor_channel` (
  `channel_id` char(45) COLLATE utf8_unicode_ci NOT NULL,
  `channel_name` varchar(45) CHARACTER SET utf8 DEFAULT NULL,
  `video_number` int(11) DEFAULT NULL,
  `view_number` int(11) DEFAULT NULL,
  `subscriber` int(11) DEFAULT NULL,
  `date_created` int(11) DEFAULT NULL,
  `status` int(11) DEFAULT NULL,
  `note` varchar(250) COLLATE utf8_unicode_ci DEFAULT NULL,
  PRIMARY KEY (`channel_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `upload_cluster`
--

DROP TABLE IF EXISTS `upload_cluster`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `upload_cluster` (
  `Id` int(11) NOT NULL AUTO_INCREMENT,
  `ClusterId` varchar(150) CHARACTER SET utf8 NOT NULL,
  `ClusterName` varchar(150) CHARACTER SET utf8 DEFAULT NULL,
  `IpAddress` varchar(150) CHARACTER SET utf8 DEFAULT NULL,
  `Port` int(5) DEFAULT NULL,
  PRIMARY KEY (`Id`)
) ENGINE=InnoDB AUTO_INCREMENT=12 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `video_container`
--

DROP TABLE IF EXISTS `video_container`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `video_container` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `video_id` varchar(15) COLLATE utf8_unicode_ci NOT NULL,
  `title` varchar(150) CHARACTER SET utf8 DEFAULT NULL,
  `description` varchar(4096) CHARACTER SET utf8 DEFAULT NULL,
  `tag` varchar(1024) CHARACTER SET utf8 DEFAULT NULL,
  `thumbnail` varchar(150) CHARACTER SET utf8 DEFAULT NULL,
  `downloaded_path` varchar(150) CHARACTER SET utf8 DEFAULT NULL,
  `rendered_path` varchar(150) CHARACTER SET utf8 DEFAULT NULL,
  `process_status` int(11) DEFAULT NULL,
  `license` int(11) DEFAULT NULL,
  `mapping_list_id` int(11) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `fk_video_container_mapping_list1_idx` (`mapping_list_id`),
  CONSTRAINT `fk_video_container_mapping_list1` FOREIGN KEY (`mapping_list_id`) REFERENCES `mapping_list` (`id`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB AUTO_INCREMENT=84 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2018-10-18  9:07:06
