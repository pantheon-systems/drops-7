(function ($) {

Drupal.behaviors.fieldablePanelPaneFieldsetSummaries = {
  attach: function (context) {
    $('fieldset.vertical-tabs-pane.fieldable-pane-pane-form-reusable-information', context).drupalSetSummary(function (context) {
      var summary = [];

      if ($('input[name="reusable"]', context).is(':checked')) {
        summary.push(Drupal.t('Reusable: Yes'));
        var category = $('input[name="category"]', context).val() || Drupal.t('None');
        summary.push(Drupal.t('Category: @value', { '@value': category }));
        var admin_title = $('input[name="admin_title"]', context).val() || Drupal.t('None');
        summary.push(Drupal.t('Admin title: @value', { '@value': admin_title }));
      }
      else {
        summary.push(Drupal.t('Reusable: No'));
      }

      return summary.join('<br />');
    });

    $('fieldset.vertical-tabs-pane.fieldable-pane-pane-form-revision-information', context).drupalSetSummary(function (context) {
      var revisionCheckbox = $('input[name="revision"]', context);

      // Return 'New revision' if the 'Create new revision' checkbox is checked,
      // or if the checkbox doesn't exist, but the revision log does. For users
      // without the "Administer content" permission the checkbox won't appear,
      // but the revision log will if the content type is set to auto-revision.
      if (revisionCheckbox.is(':checked') || (!revisionCheckbox.length && $('.form-item-log textarea', context).length)) {
        return Drupal.t('New revision');
      }

      return Drupal.t('No revision');
    });
  }
};

})(jQuery);
