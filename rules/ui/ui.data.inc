<?php
// $Id$

/**
 * @file Contains data type related forms.
 */


/**
 * Interface for data types providing a direct input form.
 */
interface RulesDataDirectInputForm {

  /**
   * @return Array
   *  The direct input form.
   */
  public function inputForm($name, $info, $settings, RulesPlugin $element);
}

/**
 * Default UI related class for data types.
 */
class RulesDataUI {

  /**
   * Specifies the default input mode per data type.
   */
  public function getDefaultMode() {
    return 'selector';
  }

  /**
   * Provides the selection form for a parameter.
   */
  public function selectionForm($name, $info, $settings, RulesPlugin $element) {
    $form[$name . ':select'] = array(
      '#type' => 'textfield',
      '#title' => t('Data selector'),
      '#default_value' => isset($settings[$name . ':select']) ? $settings[$name . ':select'] : '',
      '#required' => empty($info['optional']),
    );
    $settings += array($name . ':process' => array());
    $form[$name . ':process'] = array();
    RulesDataProcessor::attachForm($form[$name . ':process'], $settings[$name . ':process'], $info, $element->availableVariables());
    return $form;
  }
}

/**
 * UI for textual data.
 */
class RulesDataUIText extends RulesDataUI implements RulesDataDirectInputForm {

  public function getDefaultMode() {
    return 'input';
  }

  public function inputForm($name, $info, $settings, RulesPlugin $element) {
    if (!empty($info['options list'])) {
      // Make sure the .rules.inc of the providing module is included as the
      // options list callback may reside there.
      $element->call('loadBasicInclude');
      $form[$name] = array(
        '#type' => 'select',
        '#options' => $info['options list']($element),
      );
    }
    else {
      $form[$name] = array(
        '#type' => 'textarea',
      );
      RulesDataInputEvaluator::attachForm($form, $settings, $info, $element->availableVariables());
    }
    $form[$name] += array(
      '#title' => t('Value'),
      '#default_value' => isset($settings[$name]) ? $settings[$name] : '',
      '#required' => empty($info['optional']),
    );
    return $form;
  }
}

/**
 * UI for decimal data.
 */
class RulesDataUIDecimal extends RulesDataUIText {

  public function inputForm($name, $info, $settings, RulesPlugin $element) {
    $form = parent::inputForm($name, $info, $settings, $element);
    $form[$name]['#type'] = 'textfield';
    $form[$name]['#element_validate'][] = 'rules_ui_element_decimal_validate';
    return $form;
  }
}

/**
 * UI for integers.
 */
class RulesDataUIInteger extends RulesDataUIText {

  public function inputForm($name, $info, $settings, RulesPlugin $element) {
    $form = parent::inputForm($name, $info, $settings, $element);
    $form[$name]['#type'] = 'textfield';
    $form[$name]['#element_validate'][] = 'rules_ui_element_integer_validate';
    return $form;
  }
}

/**
 * UI for boolean data.
 */
class RulesDataUIBoolean extends RulesDataUI implements RulesDataDirectInputForm {

  public function getDefaultMode() {
    return 'input';
  }

  public function inputForm($name, $info, $settings, RulesPlugin $element) {
    $form[$name] = array(
      '#type' => 'checkbox',
      '#title' => check_plain($info['label']),
      '#default_value' => !empty($settings[$name]),
    );
    return $form;
  }
}

/**
 * UI for dates.
 */
class RulesDataUIDate extends RulesDataUIText {

  public function inputForm($name, $info, $settings, RulesPlugin $element) {
    $settings += array($name => gmdate('Y-m-d H:i:s', time()));
    // Convert any configured timestamp into a readable format.
    if (is_numeric($settings[$name])) {
      $settings[$name] = gmdate('Y-m-d H:i:s', $settings[$name]);
    }
    $form = parent::inputForm($name, $info, $settings, $element);
    $form[$name]['#type'] = 'textfield';
    $form[$name]['#element_validate'][] = 'rules_ui_element_date_validate';
    // Note that the date input evaluator takes care for parsing dates using
    // strtotime() into a timestamp, which is the internal date format.
    $form[$name]['#description'] = t('The date in GMT. Format: %format or other values in GMT known by the PHP !strtotime function like "+1 day". Relative dates like "+1 day" or "now" relate to the evaluation time.', array('%format' => gmdate('Y-m-d H:i:s', time() + 86400), '!strtotime' => l('strtotime()', 'http://php.net/strtotime')));

    //TODO: Leverage the jquery datepicker+timepicker once a module providing
    //the timpeicker is available.
    return $form;
  }
}

/**
 * UI for duration type parameter.
 */
class RulesDataUIDuration extends RulesDataUIText {

  public function inputForm($name, $info, $settings, RulesPlugin $element) {
    $form = parent::inputForm($name, $info, $settings, $element);
    $form[$name]['#type'] = 'textfield';
    $form[$name]['#size'] = 8;
    $form[$name]['#element_validate'][] = 'rules_ui_element_integer_validate';
    $form['multiplier'] = array(
      '#type' => 'select',
      '#options' => self::multipliers(),
      '#default_value' => 1,
    );

    // Make sure it looks like a single form element.
    $form['#theme_wrappers'] = array('form_element');
    $form['#title'] = $form[$name]['#title'];
    $form[$name]['#title_display'] = 'none';

    $form[$name]['#prefix'] = '<div class="rules-duration container-inline">';
    $form['multiplier']['#suffix'] = '</div>';

    // Set an appropriate multiplier.
    foreach (array_keys(self::multipliers()) as $m) {
      if ($form[$name]['#default_value'] % $m == 0) {
        $form['multiplier']['#default_value'] = $m;
      }
    }
    // Divide value by the multiplier, so the display is correct.
    $form[$name]['#default_value'] /= $form['multiplier']['#default_value'];
    // Specify an after build callback that transforms the value back.
    $form['#after_build'][] = 'rules_ui_element_duration_after_build';
    return $form;
  }

  public static function multipliers() {
    return array(
      1 => t('seconds'),
      60 => t('minutes'),
      3600 => t('hours'),
      86400 => t('days'),
    );
  }
}

/**
 * UI for the URI type parameter.
 */
class RulesDataUIURI extends RulesDataUIText {

  public function inputForm($name, $info, $settings, RulesPlugin $element) {
    $form = parent::inputForm($name, $info, $settings, $element);
    $form[$name]['#rows'] = 2;
    return $form;
  }

}