<?php if (theme_get_setting('scrolltop_display')): ?>
<div id="toTop"><span class="glyphicon glyphicon-chevron-up"></span></div>
<?php endif; ?>

<?php if ($page['header_top_left'] || $page['header_top_right']) :?>
<!-- #header-top -->
<div id="header-top" class="clearfix">
    <div class="container">

        <!-- #header-top-inside -->
        <div id="header-top-inside" class="clearfix">
            <div class="row">
            
            <?php if ($page['header_top_left']) :?>
            <div class="<?php print $header_top_left_grid_class; ?>">
                <!-- #header-top-left -->
                <div id="header-top-left" class="clearfix">
                    <?php print render($page['header_top_left']); ?>
                </div>
                <!-- EOF:#header-top-left -->
            </div>
            <?php endif; ?>
            
            <?php if ($page['header_top_right']) :?>
            <div class="<?php print $header_top_right_grid_class; ?>">
                <!-- #header-top-right -->
                <div id="header-top-right" class="clearfix">
                    <?php print render($page['header_top_right']); ?>
                </div>
                <!-- EOF:#header-top-right -->
            </div>
            <?php endif; ?>
            
            </div>
        </div>
        <!-- EOF: #header-top-inside -->

    </div>
</div>
<!-- EOF: #header-top -->    
<?php endif; ?>

<!-- header -->
<header id="header" role="banner" class="clearfix">
    <div class="container">

        <!-- #header-inside -->
        <div id="header-inside" class="clearfix">
            <div class="row">
                <div class="col-md-3 clearfix">

                  <?php if ($logo):?>
                  <div id="logo">
                  <a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>" rel="home"> <img src="<?php print $logo; ?>" alt="<?php print t('Home'); ?>" /> </a>
                  </div>
                  <?php endif; ?>

                  <?php if ($site_name):?>
                  <div id="site-name">
                  <a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>"><?php print $site_name; ?></a>
                  </div>
                  <?php endif; ?>
                
                  <?php if ($site_slogan):?>
                  <div id="site-slogan">
                  <?php print $site_slogan; ?>
                  </div>
                  <?php endif; ?>
                  
                </div>
                  
                <div class="col-md-9">
                    <!-- #header -->
				    <?php if ($page['header']) :?>
                    <div id="header-region" class="clearfix">
                       <?php print render($page['header']); ?>
                    </div>
                    <?php endif; ?>
                    <!-- EOF:#header -->
                    
                    <!-- Main Menu -->
                    <div id="main-menu">
                      <div class="navbar">
                        <div id="navbar-mainmenu-collapse">
                          <nav id="main-navigation" class="">
                            <?php if ($page['main_navigation']) :?>
                              <?php print drupal_render($page['main_navigation']); ?>
                            <?php else : ?>
                              <?php print theme('links__system_main_menu', array('links' => $main_menu, 'attributes' => array('class' => array('main-menu', 'menu'), ), 'heading' => array('text' => t('Main menu'), 'level' => 'h2', 'class' => array('element-invisible'), ), )); ?>
                            <?php endif; ?>
                          </nav>
                        </div>
                      </div>
                    </div>
                    <!-- End Menu -->
                </div>
                
            </div>
        </div>
        <!-- EOF: #header-inside -->

    </div>
</header>
<!-- EOF: #header --> 

<!-- #Search -->
<div id="search-wrapper" class="clearfix">
  <div class="container">
    <div class="row"><div class="col-md-12">
      <div id="search-region" class="clearfix">
        <?php if ($page['search']) :?>        
          <div id="navbar-search-collapse">
              <?php print render($page['search']); ?>
          </div>
        <?php endif; ?>
      </div>
    </div></div>
  </div>
</div>
<!-- #Search -->

<!-- #Full width -->
<?php if ($page['full_width']):?>
<div id="full-width-wrapper" class="clearfix">
		<?php print render($page['full_width']); ?>
</div>
<?php endif; ?>
<!-- EOF: #Full width -->

<?php if ($page['preface_first'] || $page['preface_second']) :?>
<!-- #Preface -->
<div id="preface" class="clearfix">
    <div class="container">

        <!-- #preface-inside -->
        <div id="preface-inside" class="clearfix">
            <div class="row">
            
            <?php if ($page['preface_first']) :?>
            <div class="<?php print $preface_first_grid_class; ?>">
                <!-- #preface-first -->
                <div id="preface-first" class="clearfix">
                    <?php print render($page['preface_first']); ?>
                </div>
                <!-- EOF:#preface-first -->
            </div>
            <?php endif; ?>
            
            <?php if ($page['preface_second']) :?>
            <div class="<?php print $preface_second_grid_class; ?>">
                <!-- ##Preface-Second -->
                <div id="preface-second" class="clearfix">
                    <?php print render($page['preface_second']); ?>
                </div>
                <!-- EOF:#Preface-Second -->
            </div>
            <?php endif; ?>
            
            </div>
        </div>
        <!-- EOF: #Preface-inside -->

    </div>
</div>
<!-- EOF: #Preface -->    
<?php endif; ?>

<!-- #page -->
<div id="page" class="clearfix">

    <?php if ($page['content_top_first']):?>
    <div id="content-top-first-wrapper" class="clearfix">
    	<div class="container">
        	<div class="row">
                <!-- EOF:#content-top-first-wrapper -->
                <section id="content-top-first" class="col-md-12 content-top-first clearfix">
                    <?php print render($page['content_top_first']); ?>
                </section>
                <!-- EOF:#content-top-first-wrapper -->
    		</div>
    	</div>
    </div>
	<?php endif; ?>

    <?php if ($page['content_top_second']):?>
    <div id="content-top-second-wrapper" class="clearfix">
    	<div class="container">
        	<div class="row">
                <!-- EOF:#content-top-second-wrapper -->
                <section id="content-top-second" class="col-md-12 content-top-second clearfix">
                    <?php print render($page['content_top_second']); ?>
                </section>
                <!-- EOF:#content-top-second-wrapper -->
    		</div>
    	</div>
    </div>
	<?php endif; ?>

    <?php if ($page['content_top_third']):?>
    <div id="content-top-third-wrapper" class="clearfix">
    	<div class="container">
        	<div class="row">
                <!-- EOF:#content-top-third-wrapper -->
                <section id="content-top-third" class="col-md-12 content-top-third clearfix">
                    <?php print render($page['content_top_third']); ?>
                </section>
                <!-- EOF:#content-top-third-wrapper -->
    		</div>
    	</div>
    </div>
	<?php endif; ?>

    <?php if ($page['image_background_top']):?>
    <!-- EOF:#image-background-wrapper -->
    <div id="image-background-top-wrapper" class="image-background-wrapper clearfix">
    	<div class="container">
        	<div class="row">
                <section id="image-background-top" class="col-md-12 image-background image-background-top clearfix">
                    <?php print render($page['image_background_top']); ?>
                </section>
    		</div>
    	</div>
    </div>
    <!-- EOF:#image-background-wrapper -->
	<?php endif; ?>
    
    <!-- #main-content -->
    <div id="main-content">
        <div class="container">
        
            <!-- #messages-console -->
            <?php if ($messages):?>
            <div id="messages-console" class="clearfix">
                <div class="row">
                    <div class="col-md-12">
                    <?php print $messages; ?>
                    </div>
                </div>
            </div>
            <?php endif; ?>
            <!-- EOF: #messages-console -->
            
            <div class="row">

                <?php if ($page['sidebar_first']):?>
                <aside class="<?php print $sidebar_grid_class; ?>">  
                    <!--#sidebar-first-->
                    <section id="sidebar-first" class="sidebar clearfix">
                    <?php print render($page['sidebar_first']); ?>
                    </section>
                    <!--EOF:#sidebar-first-->
                </aside>
                <?php endif; ?>


                <section class="<?php print $main_grid_class; ?>">

                    <!-- #main -->
                    <div id="main" class="clearfix">
                    
                        <?php if ($breadcrumb && theme_get_setting('breadcrumb_display')):?> 
                        <!-- #breadcrumb -->
                        <div id="breadcrumb" class="clearfix">
                            <!-- #breadcrumb-inside -->
                            <div id="breadcrumb-inside" class="clearfix">
                            <?php print $breadcrumb; ?>
                            </div>
                            <!-- EOF: #breadcrumb-inside -->
                        </div>
                        <!-- EOF: #breadcrumb -->
                        <?php endif; ?>

                        <!-- EOF:#content-wrapper -->
                        <div id="content-wrapper">

                            <?php print render($title_prefix); ?>
                            <?php if ($title):?>
                            <h1 class="page-title"><?php print $title; ?></h1>
                            <?php endif; ?>
                            <?php print render($title_suffix); ?>

                            <?php print render($page['help']); ?>
                      
                            <!-- #tabs -->
                            <?php if ($tabs):?>
                                <div class="tabs">
                                <?php print render($tabs); ?>
                                </div>
                            <?php endif; ?>
                            <!-- EOF: #tabs -->

                            <!-- #action links -->
                            <?php if ($action_links):?>
                                <ul class="action-links">
                                <?php print render($action_links); ?>
                                </ul>
                            <?php endif; ?>
                            <!-- EOF: #action links -->

                            <?php print render($page['content']); ?>
                            <?php print $feed_icons; ?>

                        </div>
                        <!-- EOF:#content-wrapper -->

                        
                    </div>
                    <!-- EOF:#main -->

                </section>

                <?php if ($page['sidebar_second']):?>
                <aside class="<?php print $sidebar_grid_class; ?>">
                    <!--#sidebar-second-->
                    <section id="sidebar-second" class="sidebar clearfix">
                    <?php print render($page['sidebar_second']); ?>
                    </section>
                    <!--EOF:#sidebar-second-->
                </aside>
                <?php endif; ?>

        
            </div>
        </div>
    </div>
    <!-- EOF:#main-content -->

    <?php if ($page['color_background']):?>
    <!-- EOF:#color-background-wrapper -->
    <div id="color-background-wrapper" class="clearfix">
    	<div class="container">
        	<div class="row">
                <section id="color-background" class="col-md-12 color-background clearfix">
                    <?php print render($page['color_background']); ?>
                </section>
    		</div>
    	</div>
    </div>
    <!-- EOF:#color-background-wrapper -->
	<?php endif; ?>
    
    <?php if ($page['content_bottom_first']):?>
    <div id="content-bottom-first-wrapper" class="clearfix">
    	<div class="container">
        	<div class="row">
                <!-- EOF:#content-bottom-first-wrapper -->
                <section id="content-bottom-first" class="col-md-12 content-bottom-first clearfix">
                    <?php print render($page['content_bottom_first']); ?>
                </section>
                <!-- EOF:#content-bottom-first-wrapper -->
    		</div>
    	</div>
    </div>
	<?php endif; ?>
    
    
    <?php if ($page['content_bottom_second']):?>
    <div id="content-bottom-second-wrapper" class="clearfix">
    	<div class="container">
        	<div class="row">
                <!-- EOF:#content-bottom-second-wrapper -->
                <section id="content-bottom-second" class="col-md-12 content-bottom-second clearfix">
                    <?php print render($page['content_bottom_second']); ?>
                </section>
                <!-- EOF:#content-bottom-second-wrapper -->
    		</div>
    	</div>
    </div>
	<?php endif; ?>

    <?php if ($page['image_background']):?>
    <!-- EOF:#image-background-wrapper -->
    <div id="image-background-wrapper" class="image-background-wrapper clearfix">
    	<div class="container">
        	<div class="row">
                <section id="image-background" class="col-md-12 image-background clearfix">
                    <?php print render($page['image_background']); ?>
                </section>
    		</div>
    	</div>
    </div>
    <!-- EOF:#image-background-wrapper -->
	<?php endif; ?>

    <?php if ($page['content_bottom_third']):?>
    <div id="content-bottom-third-wrapper" class="clearfix">
    	<div class="container">
        	<div class="row">
                <!-- EOF:#content-bottom-third-wrapper -->
                <section id="content-bottom-third" class="col-md-12 content-bottom-third clearfix">
                    <?php print render($page['content_bottom_third']); ?>
                </section>
                <!-- EOF:#content-bottom-third-wrapper -->
    		</div>
    	</div>
    </div>
	<?php endif; ?>

    <?php if ($page['content_bottom_fourth']):?>
    <div id="content-bottom-fourth-wrapper" class="clearfix">
    	<div class="container">
        	<div class="row">
                <!-- EOF:#content-bottom-fourth-wrapper -->
                <section id="content-bottom-fourth" class="col-md-12 content-bottom-fourth clearfix">
                    <?php print render($page['content_bottom_fourth']); ?>
                </section>
                <!-- EOF:#content-bottom-fourth-wrapper -->
    		</div>
    	</div>
    </div>
	<?php endif; ?>

    <?php if ($page['image_background_second']):?>
    <!-- EOF:#image-background-second-wrapper -->
    <div id="image-background-second-wrapper" class="image-background-wrapper image-background-second-wrapper clearfix">
    	<div class="container">
        	<div class="row">
                <section id="image-background-second" class="col-md-12 image-background clearfix">
                    <?php print render($page['image_background_second']); ?>
                </section>
    		</div>
    	</div>
    </div>
    <!-- EOF:#image-background-second-wrapper -->
	<?php endif; ?>

    <?php if ($page['content_bottom_fifth']):?>
    <div id="content-bottom-fifth-wrapper" class="clearfix">
    	<div class="container">
        	<div class="row">
                <!-- EOF:#content-bottom-fifth-wrapper -->
                <section id="content-bottom-fifth" class="col-md-12 content-bottom-fifth clearfix">
                    <?php print render($page['content_bottom_fifth']); ?>
                </section>
                <!-- EOF:#content-bottom-fifth-wrapper -->
    		</div>
    	</div>
    </div>
	<?php endif; ?>
    
</div>
<!-- EOF:#page -->

<?php if ($page['footer_first'] || $page['footer_second'] || $page['footer_third'] || $page['footer_fourth'] || $page['footer_fifth']):?>
<!-- #footer -->
<footer id="footer" class="clearfix">
    <div class="container">    
        <!-- #footer-inside -->
        <div id="footer-inside" class="clearfix">
            <div class="row">
                <div class="col-md-12">
                    <?php if ($page['footer_first']):?>
                    <div class="footer-first">
                    <?php print render($page['footer_first']); ?>
                    </div>
                    <?php endif; ?>
                </div>
                
                <div class="col-md-3">
                    <?php if ($page['footer_second']):?>
                    <div class="footer-second">
                    <?php print render($page['footer_second']); ?>
                    </div>
                    <?php endif; ?>
                </div>

                <div class="col-md-3">
                    <?php if ($page['footer_third']):?>
                    <div class="footer-third">
                    <?php print render($page['footer_third']); ?>
                    </div>
                    <?php endif; ?>
                </div>

                <div class="col-md-3">
                    <?php if ($page['footer_fourth']):?>
                    <div class="footer-fourth">
                    <?php print render($page['footer_fourth']); ?>
                    </div>
                    <?php endif; ?>
                </div>

                <div class="col-md-3">
                    <?php if ($page['footer_fifth']):?>
                    <div class="footer-fifth">
                    <?php print render($page['footer_fifth']); ?>
                    </div>
                    <?php endif; ?>
                </div>
            </div>
        </div>
        <!-- EOF: #footer-inside -->    
    </div>
    <div id="footer-sixth">
    <div class="container">    
        <!-- #footer-inside -->
            <div class="row">
                <div class="col-md-12">
                    <?php if ($page['footer_sixth']):?>
                    <div class="footer-sixth">
                        <?php print render($page['footer_sixth']); ?>
                    </div>
                    <?php endif; ?>
                </div>
            </div>
        </div>
        <!-- EOF: #footer-inside -->    
    </div>
    
</footer> 
<!-- EOF #footer -->
<?php endif; ?>

<footer id="sub-footer" class="clearfix">
    <div class="container">
        
        <!-- #subfooter-inside -->
        <div id="sub-footer-inside" class="clearfix">
            <div class="row">
                <div class="col-md-12">
                    <!-- #subfooter-left -->
                    <div class="sub-footer-first">
                    <?php if ($page['sub_footer_first']):?>
                    <?php print render($page['sub_footer_first']); ?>
                    <?php endif; ?>

                    </div>
                    <!-- EOF: #subfooter-left -->
                </div>
                
                <div class="col-md-6">
                    <!-- #subfooter-right -->
                    <div class="sub-footer-second">

                    <?php if ($page['sub_footer_second']):?>
                    <?php print render($page['sub_footer_second']); ?>
                    <?php endif; ?>

                    </div>
                    <!-- EOF: #subfooter-right -->
                </div>
            </div>
        </div>
        <!-- EOF: #subfooter-inside -->
    
    </div>
</footer>
<!-- EOF:#subfooter -->
