<?php
/**
 * Callback for drupal_get_form() to get the form for the admin/settings/squeeze page.
 */
function squeeze_admin_settings() {
  $form = array();
  $form['#submit'][] = 'squeeze_admin_settings_submit';

  $options = array('' => t("None"));
  $node_rows = db_query("SELECT nid, title FROM {node} n WHERE status = 1 AND EXISTS (SELECT * FROM {webform} w WHERE w.nid = n.nid)");
  while ($row = db_fetch_object($node_rows)) {
    $options[$row->nid] = $row->title;
  }

  $form['squeeze_settings_warning'] = array(
    '#type' => 'markup',
    '#markup' => t("Warning: setting a value below without checking any of the other options will make Squeeze intercept each and every node page. This including the webform it will be redirected to. This will render the site unusable. So when setting a vlaue here, you typically also want to check '@field' or resticting Squeeze to some content types only.",
       array('@field' => t("Default Squeeze form"))),
  );
  $form['squeeze_settings_form'] = array(
    '#type' => 'select',
    '#title' => t("Default Squeeze form"),
    '#description' => t("This value sets the default webform to be shown when Squeeze intercepts a page show or file download. This value can be ooverridden per node."),
    '#default_value'=> variable_get('squeeze_settings_form', 0),
    '#options' => $options,
  );
  $form['squeeze_settings_show_page'] = array(
    '#type' => 'checkbox',
    '#title' => t("Default for 'Show page?'"),
    '#description' => t("This value sets the default value for the 'Show page?' value that can be set per node. This value indicates whether to protect the page as a whole or protect only the attached files. File fields should have 'Generic files + Stats URL [+ Download Count]' as display type in the 'Display fields' tab for this content type."),
    '#default_value' => variable_get('squeeze_settings_show_page', 0),
  );
  $form['squeeze_settings_anonymous_only'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show only for anonymous users'),
    '#description' => t('Check whether you want to show the Squeeze form only to anonymous users. This is equal to giving all roles the "@permission" permission at once, except the locked "@anonymous_user" role.',
       array('@permission' => t('skip squeeze page'), '@anonymous_user' => t('anonymous user'))),
    '#default_value' => variable_get('squeeze_settings_anonymous_only', 0),
  );
  $form['squeeze_settings_redirect'] = array(
    '#type' => 'checkbox',
    '#title' => t('Redirect back to protected page'),
    '#description' => t('Check if you want the page to redirect back to the protected page after the webform is successfully submitted. Upon returning to the protected page, the download will be automatically initiated with JavaSrcipt and a message will be displayed at the top of the page with a manual download link if the automatic download does not begin.'),
    '#default_value' => variable_get('squeeze_settings_redirect', 1),
  );
  $form['squeeze_settings_download_message'] = array(
    '#type' => 'textarea',
    '#title' => t('Download will start now message'),
    '#description' => t('The message displayed at the top of the page to indicate that the download will automatically begin. Note: this only applies if "Redirect back to protected page" is selected.'),
    '#default_value' => variable_get('squeeze_settings_download_message', t('Your file download will begin shortly. If it does not, you can manually download your file.')),
  );
  $form['squeeze_settings_content_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Squeeze enabled content types'),
    '#description' => t('Squeeze allows you to restrict its interception to specific content types. Choose the types on which you would like to use Squeeze. If you do not select any content type, Squeeze will be enabled for all content types.'),
    '#options' => node_get_types('names'),
    '#default_value' => variable_get('squeeze_settings_content_types', array()),
  );
  return system_settings_form($form);
}

/**
 * Form submit callback for the admin_settings form.
 *
 * Processes the submitted form. Settings are handled by the default system
 * settings form handling, but we clean up the representation of supported
 * content types here before storing it in a variable.
 */
function squeeze_admin_settings_submit($form, &$form_state) {
  $form_state['values']['squeeze_settings_content_types'] = array_keys(array_filter($form_state['values']['squeeze_settings_content_types']));
}
