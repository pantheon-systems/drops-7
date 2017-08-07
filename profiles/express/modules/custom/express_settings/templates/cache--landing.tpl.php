<p>To ensure that sites hosted on the Web Express platform load quickly, web pages, images, and files are cached or stored temporarily at a few different levels.</p>

<p>Our caches are designed to keep themselves current and expire without . Under normal conditions you won't
need to clear them, but occasionally there are problems that require clearing one or more cache.
Each time a cache is cleared, it temporarily reduces the performance of the site for editors and visitors.</p>

<h2>Which Cache to Clear?</h2>

<div class="express-cards">
  <div class="express-card col-lg-4 col-md-4 col-sm-6 col-xs-12">
    <div class="content">
      <h3 class="title-center"><i class="fa fa-laptop fa-4x" aria-hidden="true"></i><br />Browser Cache</h3>
      <p>
      Every time a user visits a website, their web browser downloads all the images, CSS files, and other static files temporarily. When the same user goes to another page on the site, it will load much faster because all the static files are in their browser cache.
      </p>
      <p>
      <b>When to clear:</b> You and your co-worker see different things when you view the same webpage on different computers when you are both logged in or just viewing the site as a regular user.
      </p>
      <p>
      <b>How to clear:</b> OIT maintains <a href="https://oit.colorado.edu/services/network-internet-services/ucb-wireless/tips-tricks/clear-browser-cache">step by step instructions for clearing browser caches for different browsers and operating systems</a>.
      </p>
    </div>
  </div>
  <div class="express-card  col-lg-4 col-md-4 col-sm-6 col-xs-12">
    <div class="content">

      <h3 class="title-center"><i class="fa fa-cloud fa-4x" aria-hidden="true"></i><br />Page Cache</h3>
      <p>
      The Page Cache is responsible for creating a static version of pages on your site and stores them for a 10 minute cycle. The purpose is to speed up the time it takes browsers to display content from your site on a user’s computer or mobile device. After 10 minutes, it refreshes the page content and starts the cycle again.
      </p>
      <p>
      <b>When to clear:</b> A page looks correct while logged in, but incorrect when you are not logged in on the same computer. In most cases you don’t need to do anything and can wait out the 10 minute cache cycle. We recommend only clearing the Page Cache if you need your changes to be reflected immediately due to the urgency of a change, or you haven’t seen the change after waiting 10-15 minutes.
      </p>
      <p>
      <b>How to clear:</b> Site Owners and Content Editors can clear the cache of any path at any time by <a href="clear/varnish-path">entering the path</a> or using the link to Clear Page Cache found next to Edit Layout on every page.  Site Owners can <a href="clear/varnish-full">clear all content from the Page Cache for the site</a> once an hour.
      </p>
    </div>
  </div>
  <div class="express-card  col-lg-4 col-md-4 col-sm-6 col-xs-12">
    <div class="content">
      <h3 class="title-center"><i class="fa fa-server fa-4x" aria-hidden="true"></i> <br />Web Express Cache</h3>
      <p>
      Database caching reduces the server load by caching content in the database. This reduces the time it takes Web Express to process requests for specific pages. In very rare cases, the database cache can impact site editors from seeing changes they are making to their website while logged in -- such as adding a new page to the main menu and not seeing the new tab on your main navigation. 
      </p>
      <p>
      <b>When to clear:</b> The need to clear the database cache is pretty rare. The only time you would ever need to do this is if you are working on your website and none of your changes are showing while you are logged in. Site owners can clear the entire database cache once per hour if needed. 
      </p>
      <p>
      <b>How to clear:</b> Site Owners can <a href="clear/drupal-full">clear the entire Database Cache for the site</a> once an hour.
      </p>
    </div>
  </div>

</div>
