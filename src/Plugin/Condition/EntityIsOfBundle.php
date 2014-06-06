<?php

/**
 * @file
 * Contains \Drupal\rules\Plugin\Condition\EntityIsOfBundle.
 */

namespace Drupal\rules\Plugin\Condition;

use Drupal\Core\TypedData\TypedData;
use Drupal\Core\TypedData\TypedDataManager;
use Drupal\rules\Context\ContextDefinition;
use Drupal\rules\Engine\RulesConditionBase;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Provides an 'Entity is of bundle' condition.
 *
 * @Condition(
 *   id = "rules_entity_is_of_bundle",
 *   label = @Translation("Entity is of bundle")
 * )
 *
 * @todo: Add access callback information from Drupal 7?
 * @todo: Add group information from Drupal 7?
 */
class EntityIsOfBundle extends RulesConditionBase {

  /**
   * Constructs an EntityIsOfBundle object.
   *
   * @param array $configuration
   *   A configuration array containing information about the plugin instance.
   * @param string $plugin_id
   *   The plugin ID for the plugin instance.
   * @param mixed $plugin_definition
   *   The plugin implementation definition.
   * @param \Drupal\Core\TypedData\TypedDataManager $typed_data_manager
   *   The typed data manager.
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition, TypedDataManager $typed_data_manager) {
    parent::__construct($configuration, $plugin_id, $plugin_definition, $typed_data_manager);
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {
    return new static(
      $configuration,
      $plugin_id,
      $plugin_definition,
      $container->get('typed_data_manager')
    );
  }

  /**
   * {@inheritdoc}
   */
  public static function contextDefinitions(TypedDataManager $typed_data_manager) {
    $contexts['entity'] = ContextDefinition::create($typed_data_manager, 'entity')
      ->setLabel(t('Entity'))
      ->setDescription(t('Specifies the entity for which to evaluate the condition.'));

    // @todo: specify input mechanisms and/or constraints once configuration/UI
    //  questions are settled.
    $contexts['type'] = ContextDefinition::create($typed_data_manager, 'string')
      ->setLabel(t('Type'))
      ->setDescription(t('The type of the evaluated entity.'));

    // @todo: specify input mechanisms and/or constraints once configuration/UI
    //  questions are settled.
    $contexts['bundle'] = ContextDefinition::create($typed_data_manager, 'string')
      ->setLabel(t('Bundle'))
      ->setDescription(t('The bundle of the evaluated entity.'));

    return $contexts;
  }


  /**
   * {@inheritdoc}
   */
  public function summary() {
    return $this->t('Entity is of bundle');
  }

  /**
   * {@inheritdoc}
   */
  public function evaluate() {
    // Load the entity to evaluate.
    $providedEntity = $this->getContextValue('entity');
    // Retrieve the entity type specified as part of this condition.
    $specified_type = $this->getContextValue('type');
    // Retrieve the entity bundle specified as part of this condition.
    $specified_bundle = $this->getContextValue('bundle');
    // Retrieve the type and bundle of the evaluated entity.
    $entity_type = $providedEntity->getEntityTypeId();
    $entity_bundle = $providedEntity->bundle();
    // Check to see whether the entity's bundle and type match the specified
    //  values.
    return $entity_bundle == $specified_bundle && $entity_type == $specified_type;
  }
}
