core = 7.x
api = 2

projects[feeds][download][type] = "git"
projects[feeds][download][url] = "http://git.drupal.org/project/feeds.git"
projects[feeds][download][revision] = e7f7f3987bd10a37010e6d1fa3d4d1b50e67c4cb
projects[feeds][download][branch] = 7.x-2.x
projects[feeds][patch][1428272] = http://drupal.org/files/feeds-encoding_support_CSV-1428272-52.patch
projects[feeds][patch][1127696] = http://drupal.org/files/issues/1127696-97.patch
projects[feeds][patch][2531706] = https://www.drupal.org/files/issues/feeds-cache-table-exists-2531706-1.patch
projects[feeds][subdir] = contrib
projects[feeds][type] = module

projects[feeds_field_fetcher][download][type] = git
projects[feeds_field_fetcher][download][url] = "http://git.drupal.org/project/feeds_field_fetcher.git"
projects[feeds_field_fetcher][download][branch] = master
projects[feeds_field_fetcher][subdir] = contrib
projects[feeds_field_fetcher][type] = module

projects[feeds_flatstore_processor][download][type] = git
projects[feeds_flatstore_processor][download][url] = "https://github.com/NuCivic/feeds_flatstore_processor.git"
projects[feeds_flatstore_processor][download][branch] = master
projects[feeds_flatstore_processor][subdir] = contrib
projects[feeds_flatstore_processor][type] = module

projects[schema][subdir] = contrib
projects[schema][download][revision] = "08b02458694d186f8ab3bd0b24fbc738f9271108"

projects[services][subdir] = contrib
projects[services][version] = 3.12  

projects[data][subdir] = contrib
projects[data][version] = 1.x

projects[job_scheduler][subdir] = contrib
projects[job_scheduler][version] = 2.x
