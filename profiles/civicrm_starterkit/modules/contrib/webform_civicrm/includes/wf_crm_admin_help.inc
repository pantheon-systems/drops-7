<?php

/**
 * @file
 * Admin form inline-help.
 */

/**
 * Class wf_crm_admin_help
 * Adding a static function to this class with the same name as a field will magically add pop-up help for that field to the admin form.
 */
class wf_crm_admin_help {

  static function intro() {
    print '<p>' .
      t('Create anything from a simple newsletter signup, to a complex multi-step registration system.') .
      '</p><strong>' .
      t('Getting Started:') .
      '</strong><ul>' .
      '<li>' . t('Enable fields for one or more contacts.') . '</li>' .
      '<li>' . t('Arrange and configure those fields on the "Webform" tab.') . '</li>' .
      '<li>' . t('Click the blue help icons to learn more.') . '</li>' .
      '<li><a href="http://wiki.civicrm.org/confluence/display/CRMDOC/Webform+CiviCRM+Integration" target="_blank">' . t('Read the instructions.') . '</a></li>' .
      '</ul>';
  }

  static function contact_existing() {
    print '<p>' .
      t('Gives many options for how this contact can be autofilled or selected. From the Webform tab you can edit this field to configure:') .'</p><ul>' .
      '<li>' . t('Widget: Determine whether to expose this field to the form as an autocomplete or select element, or hide it and pick the contact automatically.') . '</li>' .
      '<li>' . t('Default Value: Select a contact based on the current user, relationships, or other options.') . '</li>' .
      '<li>' . t('Filters: Limit the list of available choices from which this contact may be autofilled or selected.') . '</li>' .
      '<li>' . t('Show/Hide Fields: Control which other fields the user is allowed to edit and which will be hidden.') . '</li>' .
      '</ul>';
  }

  static function contact_employer_id() {
    print '<p>' .
      t('Choose a webform contact of type "Organization" to be the employer for this individual.') .
      '</p><p>' .
      t('Use the "Existing Contact" field for that organization to enable autocomplete or selection of employers.') .
      '</p><p>' .
      t('You can also autofill the employer by configuring the organization\'s "Existing Contact" default value to be "Employer" relationship to Contact 1.') .
      '</p>';
  }

  static function contact_image_URL() {
    print '<p>' .
      t('Allows an image to be associated with a contact. This image will appear in CiviCRM, but the file is stored in Drupal. If the webform submission or entire webform were to be deleted, the image would be lost.') .
      '</p>';
  }

  static function contact_contact_id() {
    print '<p>' .
      t('This hidden field can be used to as a token to generate links, for example to include an email link back to this form to update their info.') .
      '</p>';
  }

  static function contact_external_identifier() {
    self::contact_contact_id();
  }

  static function contact_source() {
    print '<p>' .
      t('This field will override the "Source Label" in "Additional Options".') .
      '</p>';
  }

  static function contact_cs() {
    self::contact_contact_id();
  }

  static function matching_rule() {
    print '<p>' .
      t('This determines how an <em>unknown</em> contact will be handled when the webform is submitted.') .
      '</p><ul>' .
      '<li>' . t('Select the "Default Unsupervised" rule for the same duplicate matching used by CiviCRM event registration &amp contribution forms.') . '</li>' .
      '<li>' . t('Select a specific rule to customize how matching is performed.') . '</li>' .
      '<li>' . t('Or select "- None -" to always create a new contact.') . '</li>' .
      '</ul><p>' .
      t('Note: Matching rules are only used if the contact is not already selected via "Existing Contact" field.') .
      '</p>';
  }

  static function address_master_id() {
    print '<p>' .
      t('When selected, will hide fields for this address and use those of the other contact.') .
      '</p><p>' .
      t('Tip: In many use-cases it is desirable to show this field as a single checkbox. You can configure that by editing the field and removing all but one option (the one this contact is allowed to share) and re-labelling it something like "Same as my address".') .
      '</p>';
  }


  static function contribution_contribution_page_id() {
    print '<p>' .
      t("It is recommended to <a !link>create a new contribution page</a> solely for webform use. When configuring the page, most options will be irrelevant (such as profiles, premiums, widgets, recurring, etc.). Only the following need to be configured:",
        array('!link' => 'target="_blank" href="' . url('civicrm/admin/contribute/add', array('query' => array('reset' => 1, 'action' => 'add'))) . '"')) .
      '</p><ul>' .
      '<li>' . t('Title: Displayed on the webform.') . '</li>' .
      '<li>' . t('Financial Type: Controls overall contribution type.') . '</li>' .
      '<li>' . t('Currency: Sets currency for all payment fields on this form.') . '</li>' .
      '<li>' . t('Payment Processors: Controls list of options here.') . '</li>' .
      '<li>' . t('Contribution Amounts: Must be enabled with a label and Allow Other Amounts checked.') . '</li>' .
      '<li>' . t('Email Receipt: Configure email receipts from this webform.') . '</li>' .
      '</ul>';
  }

  static function contribution_payment_processor_id() {
    print '<p>' .
      t('Supported payment processors enabled on the contribution page are available here. "Pay Later" option allows the user to purchase events/memberships without entering a credit card.') .
      '</p><p>' .
      t("Note that only on-site credit card processors are currently supported on Webforms. Services that redirect to an external website, such as Paypal Standard, are not supported. Recurring payments are not supported.") .
      '</p>';
  }

  static function contribution_total_amount() {
    print '<p>' .
      t('This amount will be in addition to any paid events and memberships.') .
      '</p>';
    self::fee();
  }

  static function participant_fee_amount() {
    print '<p>' .
      t('Price for this event. If multiple events or participants are registered with this field, the amount will be multiplied per-person, per-event.') .
      '</p><p>' .
      t('Note that any event prices you have configured in CiviCRM are not imported into the Webform - you will need to reconfigure them here.') .
      '</p>';
    self::fee();
  }

  static function fee() {
    print '<p>' .
      t('Once added to the webform, this field can be configured in a number of ways by changing its settings.') .
      '</p><strong>' .
      t('Possible Widgets:') .
      '</strong><ul>' .
      '<li>' . t('Number (default): Allow the user to enter an amount, optionally constrained by min, max, and increments.') . '</li>' .
      '<li>' . t('Hidden: Set the amount without giving the user a choice.') . '</li>' .
      '<li>' . t('Select/Radios: Allow the user to choose from one of several options.') . '</li>' .
      '<li>' . t('MultiSelect/Checkboxes: Each choice the user selects will be added together.') . '</li>' .
      '<li>' . t('Grid: Configure multiple items and quantities.') . '</li>' .
      '</ul>';
  }

  static function participant_reg_type() {
    print '<p>' .
      t('Registering as a group will set Contact 1 as the primary registrant. Registering participants separately gives finer control over which contacts register for what events.') .
      '</p><p>' .
      t('With only one contact on the form, there is no difference between these two options.') .
      '</p>';
  }

  static function participant_event_id() {
    print '<p>' .
      t('Events can be selected here without giving the user a choice, or this field can be added to the form ("User Select").') .
      '</p><p>' .
      t('Click the + button to choose multiple events.') .
      '</p><p>' .
      t('On the form, this field could be represented as either single or multiselect (checkboxes or radios). Note: enabling this field as a multiselect (checkboxes) should only be done if all selectable events will have the same price, role, custom data, etc.') .
      '</p><p>' .
      t('"Live Options" can be enabled to keep the field up-to-date with all your organization\'s events, or you can hand-pick the events you wish to show.') .
      '</p>';
  }

  static function participant_status_id() {
    print '<ul><li>' .
      t('In "Automatic" mode, status will be set to "Registered" (or "Pending" if the user chooses to "Pay Later" for events with a fee). The user will be able to cancel registration by re-visiting the form and de-selecting any events they are registered for.') .
      '</li><li>' .
      t('If a status is selected here, events will be autofilled only if the participant has that status.') .
      '</li><li>' .
      t('If this field is exposed to the webform ("User Select"), events will be autofilled only if the particiant status among the enabled options.') .
      '</li></ul>';
  }

  static function reg_options_show_remaining() {
    print '<p>' .
      t('Display a message at the top of the form for each event with a registration limit or past end date.') .
      '</p>';
  }

  static function reg_options_validate() {
    print '<p>' .
      t('Will not allow the form to be submitted if user registers for an event that is ended or full.') .
      '</p>';
  }

  static function reg_options_block_form() {
    print '<p>' .
      t('Hide webform if all the events for the form are full or ended.') .
      '</p>';
  }

  static function reg_options_disable_unregister() {
    print '<p>' .
      t('If this is selected, on "User Select mode", participants will not be unregistered from unchecked events.') .
      '</p>';
  }

  static function reg_options_allow_url_load() {
    print '<p>' .
      t('Allow events in "User Select" mode to be auto-filled from URL.') .
      '</p>'.'<br /><p>'.t('Example for "Register all":') .
      '<br /><code>' . url("node", array('absolute' => TRUE)) . '/{node.nid}?event1={event1.event_id},{event2.event_id}</code></p><br />'.
      t('Example for "Register separately":') .
      '<br /><code>' . url("node", array('absolute' => TRUE)) .
      '/{node.nid}?c1event1={event1.event_id},{event2.event_id}&amp;c2event1={event3.event_id}</code></p>';
  }

  static function reg_options_show_past_events() {
    print '<p>' .
      t('To also display events that have ended, choose an option for how far in the past to search.') .
      '</p>';
  }

  static function reg_options_show_future_events() {
    print '<p>' .
      t('To also display events in the future, choose an option for how far in the future to search.') .
      '</p>';
  }

  static function reg_options_title_display() {
    print '<p>' .
      t('Controls how events are displayed. Date formats can be further configured in <a !link>CiviCRM Date Settings</a>',
        array('!link' => 'target="_blank" href="' . url('civicrm/admin/setting/date', array('query' => array('reset' => 1))) . '"')
      ) .
      '</p><p>' .
      t('Note: End-date will automatically be omitted if it is the same as the start-date.') .
      '</p>';
  }

  static function membership_membership_type_id() {
    print '<p>' .
      t('Fee will be calculated automatically based on selected membership type and number of terms chosen. A contribution page must be enabled to collect fees.') .
      '</p>';
  }

  static function membership_status_id() {
    print '<p>' .
      t('If number of terms is enabled, status can be calculated automatically based on new/renewal status and payment.') .
      '</p>';
  }

  static function membership_num_terms() {
    print '<p>' .
      t('Membership dates will be filled automatically by selecting terms. This can be overridden by entering dates manually.') .
      '</p><p>' .
      t('Note: Number of terms is required to calculate membership fees for paid memberships.') .
      '</p><p>'.
      t('If you choose to enter dates manually, enabling membership fee field will provide the price. Otherwise the membership will be free') .
      '</p>';
  }

  static function membership_fee_amount() {
    print '<p>' .
      t('Price for this membership per term. If this field is enabled, the default minimum membership fee from CiviCRM membership type settings will not be loaded.') .
      '</p><p>' .
      t('Note that if this field is enabled, the default minimum membership fee from CiviCRM membership type settings will not be loaded.') .
      '</p>';
    self::fee();
  }

  static function multiselect_options() {
    print '<p>' .
      t('Click the + button to select more than one option.') .
      '</p><p>' .
      t('You may set options here and/or add this element to the webform ("User Select"). Options chosen here will be applied automatically and will not appear on the form.') .
      '</p>';
  }

  static function webform_label() {
    print '<p>' .
      t('Labels help you keep track of the role of each contact on the form. For example, you might label Contact 1 "Parent", Contact 2 "Spouse" and Contact 3 "Child".') .
      '</p><p>' .
      t("Labels do not have to be shown to the end-user. By default they will be the title of each contact's fieldset, but you may rename (or remove) fieldsets without affecting this label.") .
      '</p>';
  }

  static function activity_target_contact_id() {
    print '<p>' .
      t('Which contacts should be tagged as part of this activity?') .
      '</p>';
    self::contact_reference();
  }

  static function activity_source_contact_id() {
    print '<p>' .
      t('Choose "automatic" to have this activity attributed to the current user (or contact 1 if the user is anonymous).') .
      '</p>';
    self::contact_reference();
  }

  static function activity_assignee_contact_id() {
    civicrm_initialize();
    print '<p>';
    if (CRM_Core_BAO_Setting::getItem(CRM_Core_BAO_Setting::SYSTEM_PREFERENCES_NAME, 'activity_assignee_notification')) {
      print t('A copy of this activity will be emailed to the assignee.');
    }
    else {
      print t('Assignee notifications are currently disabled in CiviCRM; no email will be sent to the assignee.');
    }
    print '</p>';
    self::contact_reference();
  }

  static function activity_duration() {
    print '<p>' .
      t('Total time spent on this activity (in minutes).') .
      '</p>';
  }

  static function existing_activity_status() {
    print '<p>' .
      t('If a matching activity of the chosen type already exists for Contact 1, it will be autofilled and updated.') .
      '</p><p>' .
      t('Note: an activity can also be autofilled by passing "activity1", etc. in the url.') .
      '</p>';
  }

  static function existing_case_status() {
    print '<p>' .
      t('If a matching case of the chosen type already exists for the client, it will be autofilled and updated.') .
      '</p><p>' .
      t('Note: a case can also be autofilled by passing "case1", etc. in the url.') .
      '</p>';
  }

  static function existing_grant_status() {
    print '<p>' .
      t('If a matching grant of the chosen type already exists for the applicant, it will be autofilled and updated.') .
      '</p><p>' .
      t('Note: a grant can also be autofilled by passing "grant1", etc. in the url.') .
      '</p>';
  }

  static function file_on_case() {
    print '<p>' .
      t('Add this activity to either a specific case from this webform, or an already existing case based on matching criteria.') .
      '</p><p>' .
      t('These options will not open a new case; configure the "Cases" section to do so.') .
      '</p>';
  }

  static function case_medium_id() {
    print '<p>' .
      t('Medium for activities added to cases from this webform.') .
      '</p>';
  }

  static function contact_reference() {
    print '<p>' .
      t('This is a contact reference field. Webform gives you a great deal of flexibility about how this field is displayed:') .
      '</p><ul>' .
      '<li>' . t('First choose a contact on this webform as the target for this field (or add a new contact to the form for that purpose).') . '</li>' .
      '<li>' . t('To enable a selection of contacts, enable the "Existing Contact" field for the selected contact.') . '</li>' .
      '<li>' . t('This allows the contact to be selected on the form via autocomplete or dropdown select, or hidden and set to always be the same contact.') . '</li>' .
      '<li>' . t('In rare cases you might want to expose the list of webform contacts ("User Select").') . '</li>' .
      '<li>' . t('There are many more possibilities, see "Existing Contact" field help for more information.') . '</li>' .
      '</ul>';
  }

  static function fieldset_relationship() {
    print '<p>' .
      t('Relationships are created from higher-number contacts to lower-number contacts.') .
      '</p><p>' .
      t("Example: to create a relationship between Contact 3 and Contact 4, go to Contact 4's tab and select Number of Relationships: 3. This will give you the option to create relationships between Contact 4 and Contacts 1, 2, and 3, respectively.") .
      '</p>';
  }

  static function contact_creation() {
    print '<p>' .
      t('CiviCRM requires at minimum a name or email address to create a new contact.') .
      '</p><p>' .
      t("Webform contacts that do not have these fields can be used for selection of existing contacts but not creating new ones.") .
      '</p>';
  }

  static function contact_component_widget() {
    print '<ul>
      <li>' . t('Autocomplete will suggest names of contacts as the user types. Good for large numbers of contacts.') . '</li>
      <li>' . t('A select list will show all possible contacts in a dropdown menu. Good for small lists - use filters.') . '</li>
      <li>' . t('A static element will select a contact automatically without giving the user a choice. Use in conjunction with a default value setting or a cid passed in the url.') . '</li>
      </ul>';
  }

  static function contact_component_hide_fields() {
    print '<p>' .
      t('When an existing contact is selected or prepopulated, which fields should the user not be allowed to edit?') .
      '</p><p>' .
      t("This is useful for preventing changes to existing data.") .
      '</p>';
  }

  /**
   * Get help for a custom field
   */
  static function custom($param) {
    list( , $id) = explode('_', $param);
    if (!is_numeric($id)) {
      return;
    }
    civicrm_initialize();
    module_load_include('inc', 'webform_civicrm', 'includes/utils');
    $info = wf_civicrm_api('custom_field', 'getsingle', array('id' => $id));
    if (!empty($info['help_pre'])) {
      print '<p>' . $info['help_pre'] . '</p>';
    }
    if (!empty($info['help_post'])) {
      print '<p>' . $info['help_post'] . '</p>';
    }
  }

  /**
   * Get help for a fieldset
   */
  static function fieldset($param) {
    list( , $set) = explode('_', $param);
    civicrm_initialize();
    module_load_include('inc', 'webform_civicrm', 'includes/utils');
    $sets = wf_crm_get_fields('sets');
    if (!empty($sets[$set]['help_text'])) {
      print '<p>' . $sets[$set]['help_text'] . '</p>';
    }
  }

  /**
   * Copied from Core CiviCRM's templates/notifications.tpl
   * @return string
   */
  static function helpTemplate() {
    return '
    <div class="crm-container">
      <div id="crm-notification-container" style="display:none">
        <div id="crm-notification-alert" class="#{type}">
          <div class="icon ui-notify-close" title="' . t('close') . '"> </div>
          <a class="ui-notify-cross ui-notify-close" href="#" title="' . t('close') . '">x</a>
          <h1>#{title}</h1>
          <div class="notify-content">#{text}</div>
        </div>
      </div>
    </div>';
  }

  /**
   * Add Civi help icon to a FAPI field
   * @param array $field
   * @param string $topic
   * @param string|null $title
   */
  static function addHelp(&$field, $topic, $title = NULL) {
    $title = $title ? $title : $field['#title'];
    $field['#title'] .= ' &nbsp;' . self::helpIcon($topic, $title);
  }

  static function helpIcon($topic, $title) {
    return '<span class="crm-container"><a class="helpicon" href="#' . $topic . '" title="' . $title .'">&nbsp;</a></span>';
  }
}

/**
 * Ajax page callback
 * @param string $topic
 */
function wf_crm_admin_help($topic) {
  if (method_exists('wf_crm_admin_help', $topic)) {
    wf_crm_admin_help::$topic();
  }
  elseif (strpos($topic, 'custom_') === 0) {
    wf_crm_admin_help::custom($topic);
  }
  elseif (strpos($topic, 'fieldset_') === 0) {
    wf_crm_admin_help::fieldset($topic);
  }
  exit();
}
