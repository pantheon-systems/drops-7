Introduction
============

Distributions contain a carefully selected set of module versions along with
specific patches that have been tested for compatibility. These projects and
their versions are defined through .make files within the distribution and their
projects.

Individually upgrading a project could potentially break the distribution and
should be avoided unless you are an experienced developer.

Unfortunately many site administrators are not aware of this and will use the
update manager (core) to get notified of newer module release and perform the
upgrade. This can cause undo issues in the distribution project issue queue.

This module attempts to help alleviate that scenario by altering the update
status report and limiting the ability to upgrade a project through the admin
UI. Specific projects are either hidden or displayed in a nested format to show
that another project owns/defines them through .make files. Similarly those
projects are hidden from the update admin ui page to avoid them being updated
individually.

Main Features
-------------

* Works with distributions or on sites that only include components
  (e.g. Panopoly WYSIWYG) of a distribution.
* (Optionally) Alter that status of a project to -current- if the project has a
  newer release but the project version matches that of the .make file.
* (Optionally) Alter the status of a project to -not-current- if the project
  version does not match that of the .make file.
* (Optionally) Configure the X number of days to keep a project status as
  -current- when the project has a security release or is revoked.
* (Optionally) block all projects defined through .make files from getting a
  status update. This also has the affect of removing them from the update
  report.
* (Optionally) hide all projects defined through .make files from the update
  report. This differs from the previous option in that the status is still
  retrieved and only rendered if there is a version issue.
* Render projects defined through .make files nested under the parent project on
  the update report page. This offers a visual indication that a project is
  "owned" by another project.
* (Optionally) Hide projects defined through .make files from appearing on the
  update admin UI page that allows for individual updating.


Configuration
=============

Configuration options have been added to the Update (core) Manager settings
page (admin/reports/updates/settings). These options configure the features
marked as -optionally- in the feature list above.


Maintainers
===========

* Craig Aschbrenner <https://www.drupal.org/user/246322>
