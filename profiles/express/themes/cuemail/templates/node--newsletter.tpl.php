<?php
  global $base_url;
  $theme_path = drupal_get_path('theme', 'cuemail');
  $path = $base_url . '/' . $theme_path;
?>
<?php hide($content['field_ememo_news']); ?>
<?php hide($content['field_ememo_block']); ?>
<?php hide($content['field_ememo_greeting']); ?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
	<meta name="viewport" content="width=device-width"/>
</head>
<body>
  <div class="emailteaser" style="display:none !important;">
		<?php print render($content['teaser']); ?>
	</div>
	<table class="body">
		<tr>
			<td class="center" align="center" valign="top">
        <table class="row title">
          <tr>
            <td class="email-top">
              <center>
                <table class="container">
                  <tr>
                    <td>
                      <table class=" full">
                        <tr>
                          <?php if (!empty($newsletter_logo_url)): ?>
                            <td class="newsletter-name">
                              <h1><img src="<?php print $newsletter_logo_url; ?>" alt="<?php print $newsletter_name; ?>" width="<?php print $newsletter_logo_width; ?>" height="<?php print $newsletter_logo_height; ?>" /></h1>
                            </td>
                          <?php else: ?>
                            <td class="newsletter-name">
                                <h1><?php print $newsletter_name; ?></h1>
                            </td>
                          <?php endif; ?>
                          <td class="newsletter-logo">
                            <img src="<?php print $path; ?>/images/<?php print cuemail_logo_color($design); ?>" alt="University of Colorado Boulder " id="logo" width="212" height="32" />
                          </td>
                        </tr>
                      </table>
                      <table class="issue full">
      									<tr>
      										<td class="issue-date">

      											<strong><?php print $ap_date_cu_medium_date; ?></strong>
      										</td>
      										<td class="web-link">

                            <?php print l('View on website', 'node/' . $node->nid, array('absolute' => TRUE)); ?>

      										</td>
      									</tr>
      								</table>
                    </td>
                  </tr>
                </table>
              </center>
            </td>
          </tr>
        </table>
        <div class="title-bottom">

        </div>
				<center>
          <?php if (isset($_GET['issue-contents']) && ($_GET['issue-contents'] == 1)): ?>
            <table class="container content-top">
              <tr>
                <td class="article-list">
                  <strong>In this issue:</strong>
                  <?php
                    print $attached_articles;
                  ?>
                </td>
              </tr>
            </table>
          <?php endif; ?>

          <?php if (!empty($content['field_newsletter_intro_image']) || !empty($content['body'][0]['#markup'])): ?>
            <?php $has_intro = TRUE; ?>
            <!-- Intro -->
            <table class="container content-top">
              <tr>
                <td class="hero-image">
                  <?php
                    $intro = theme('newsletter_intro', array('content' => $content));
                    print $intro;

                  ?>
                </td>
              </tr>
            </table>
        <?php endif; ?>


					<!-- start issue contents -->
					<table class="container <?php if (!isset($has_intro) || !$has_intro) { print 'content-top'; } ?>">
						<tr>
							<td class="newsletter-sections">
                <?php print render($content['field_newsletter_section']); ?>
							</td>
            </tr>
          </table>

          <!-- START AD 1 -->
          <?php if (!empty($content['field_newsletter_ad_promo'][0])) : ?>
            <table class="container">
  						<tr>
  							<td>
                  <?php
                    $ad_1 = theme('newsletter_section_ads', array('content' => $content['field_newsletter_ad_promo'][0], 'class' => 'ad-1'));
                    print $ad_1;

                  ?>
              </tr>
            </table>
          <?php endif; ?>
          <!-- END AD 1 -->

          <!-- START BLOCKS -->
          <?php if (!empty($content['field_newsletter_text_block'])): ?>
            <table class="container email-blocks">
  						<tr>
  							<td>
                  <?php
                    $blocks = theme('newsletter_section_blocks', array('content' => $content));
                    print $blocks;
                  ?>
                </td>
              </tr>
            </table>
          <?php endif; ?>
          <!-- END BLOCKS -->

          <!-- START AD 2 -->
          <?php if (!empty($content['field_newsletter_ad_promo'][1])) : ?>
            <table class="container">
  						<tr>
  							<td>
                  <?php
                    $ad_2 = theme('newsletter_section_ads', array('content' => $content['field_newsletter_ad_promo'][1], 'class' => 'ad-2'));
                    print $ad_2;
                  ?>
              </tr>
            </table>
          <?php endif; ?>
          <!-- END AD 2 -->

          <table class="container email-footer">
            <tr>
              <td>

                <table class="row footer">
                  <td class="wrapper last">
                    <table class="twelve columns">
                      <tr>
                        <td class="footer-content text-pad padding-bottom">
                          <p><strong><a href="<?php print $base_url; ?>"><?php print variable_get('site_name', ''); ?></a></strong></p>
                          <!--
                            <p class="copyright">&copy; Regents of the University of Colorado</p>
                          -->
                          <?php if (!empty($content['field_newsletter_footer'])): ?>
                            <div class="newsletter-footer">
                              <?php
                                print render($content['field_newsletter_footer']);
                                ?>
                            </div>
                          <?php endif; ?>
                          <div class="open-counter">
                            <custom name="opencounter" type="tracking">
                          </div>
                          <!-- don't ask... -->
                          &nbsp;&nbsp;&nbsp;&nbsp;
                          &nbsp;&nbsp;&nbsp;&nbsp;
                          &nbsp;&nbsp;&nbsp;&nbsp;
                          &nbsp;&nbsp;&nbsp;&nbsp;
                          &nbsp;&nbsp;&nbsp;&nbsp;
                          &nbsp;&nbsp;&nbsp;&nbsp;
                          &nbsp;&nbsp;&nbsp;&nbsp;
                          &nbsp;&nbsp;&nbsp;&nbsp;
                          &nbsp;&nbsp;&nbsp;&nbsp;
                          &nbsp;&nbsp;&nbsp;&nbsp;
                          &nbsp;&nbsp;&nbsp;&nbsp;
                          &nbsp;&nbsp;&nbsp;&nbsp;
                          &nbsp;&nbsp;&nbsp;&nbsp;
                          &nbsp;&nbsp;&nbsp;&nbsp;
                          &nbsp;&nbsp;&nbsp;&nbsp;
                          &nbsp;&nbsp;&nbsp;&nbsp;
                          &nbsp;&nbsp;&nbsp;&nbsp;
                          &nbsp;&nbsp;&nbsp;&nbsp;

                        </td>
                        <td class="expander"></td>
                      </tr>
                    </table>
                  </td>
                </table>
              </td>
            </tr>
          </table>
				</center>
			</td>
		</tr>
	</table>
</body>
</html>
