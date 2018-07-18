Match Redirect
--------------

This module provides redirecting based on path patterns with wildcards. This 
functions much like how block page visibility works. You specify a pattern like 
"old-blog/*" and a target like "new-blog" and all pages under old-blog will be 
redirected to new-blog.

Features
--------

* Pattern matching allowing wildcard redirects
* Redirect code choice (301, 302, etc)
* Filters out existing pages so they won't be redirected (unless overridden)
* Loop protection (no chaining redirects allowed)
