
/**
 *  @file
 *  Provide a star rating widget for rating apps
 */

(function ($) {
  // Define our widget
  /**
   *  Class for creating a voting widget
   *  @param url - the url for voting
   *  @param holder - the parent element to place the widget in
   *  @param previousVote - the previous vote by this user/site
   */
  function AppsVotingWidget (url, holder, average, userVote) {
    //create an object to store our state
    var that = {
      url : url,
      userVote : userVote,
      average : average,
      /**
       *  Function to send vote to the appserver
       *  Vote is the vote - a number between 0 and 100
       */
      doVote : function (vote) {
        if (vote != that.userVote) {
          //do an ajax call to our url
          $.ajax({
            url : url + vote,
            context : that,
            success : function (data, status, xhr) {
              if (!data.error) {
                that.updateWidget(data, vote);
              } else {
                that.showError('Error recording vote', data);
              }
            },
            error : function (xhr, status, error) {
              that.showError('Ajax Error['+status+']: ', xhr);
            }
          });
        }
      },
      /**
       *  Function to update each part of the widget when a vote is sent
       */
      updateWidget : function(data, user) {
        that.userVote = user;
        that.average = data.message.average;
        if(!that.over){
          that.setPosition(that.average);
        }
        
        var userRating = $('.app-user-rating', holder);
        userRating.removeClass('no-vote');
        //update the user's stars
        var stars = Math.round(user / 2) / 10;
        $('.stars-count', userRating).text(stars);
        //update the average stars also
        var stars = Math.round(data.message.average / 2) / 10;
        $('.app-average-rating .stars-count', holder).text(stars);
        
        $('.app-rating-count .rating-count', holder).text(data.message.count);

        //clear the error if it exists
        $('#apps-voting-error').remove();
      },
      /**
       *  Abstracts setting the stars position (controls how many stars appear)
       */
      setPosition : function(percent) {
        $('.app-stars', holder).css('width', percent);
      },
      showError : function(message, object) {
        that.log(message, object);
        $('#apps-voting-error').remove();
        $(holder).append('<div id="apps-voting-error" class="messages error">' 
          + Drupal.t('Failed to save vote. Please try again later.') + '</div>');
      },
      /**
       *  Debug logging
       */
      log : function(message, object) {
        if (typeof(console) != 'undefined' && typeof(console.log) == 'function') {
          console.log(message, object);
        }
      }
    };

    //attach the event handlers for the voting widget
    $('.app-stars-holder', holder)
      .mousemove(function(e) { //move the stars around when you mouseover
        //this is the current element
        var left = e.pageX - $(this).offset().left;
        that.setPosition(left);
        that.over = true;
      }) 
      .mouseout(function(e) { //reset the stars to the proper place when you mouse out
        that.over = false;
        that.setPosition(that.average);
      })
      .click(function(e){ //register a vote on click
        var vote = parseInt($('.app-stars', this).css('width'));
        that.doVote(vote);
      });

    //return our object
    return that;
  }

  // Create a drupal behavior to attach a new AppVotingWidget to each voting element
  Drupal.behaviors.apps_voting = {
    attach : function (context, settings) {
      $('.app-rating', context).each(function(index, el) {
        // get the settings for this element
        var machine_name = el.id.replace('app-rating-', '');
        var data = settings.apps[machine_name];
        var userRating = (data.rating.user === false) ? false : data.rating.user;
        //store a reference 
        el.appVotingWidget = AppsVotingWidget(data['url'], el, data.rating.average, userRating);
      });
    }
  };
}(jQuery));
