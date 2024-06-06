<?php
/*
  This code is part of FusionDirectory (http://www.fusiondirectory.org/)
  Copyright (C) 2016-2017  FusionDirectory

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */


class accessWebApplication extends simplePlugin
{

  protected $displayHeader  = TRUE;
  protected $loaded = FALSE;
  public $protocol = "CAS";
  // handle contain remote Access Management Service
  private $remoteAccessHandle;

  static function plInfo (): array
  {
    return [
      'plShortName'   => _('Access Web Application'),
      'plDescription' => _('Access Web Application'),
      'plPriority'    => 30,
      'plObjectClass' => ['fdAccessApplication'],
      'plObjectType'  => ['webApplication'],
      'plProvidedAcls'  => parent::generatePlProvidedAcls(static::getAttributesInfo())
    ];
  }


  private function createAccessConnection(){
    global $config;
    $ldap = $config->get_ldap_link();
    $ldap->cd($this->fdAccessApplicationAuthServerDn);
    $ldap->search('(objectClass=fdAccessService)', ['fdAccessServiceType','fdAccessServiceURL','fdAccessServiceUser','fdAccessServicePassword']);
    $attrs = $ldap->fetch();
    $serviceClass="accessMethod".$attrs['fdAccessServiceType'][0];
    $this->remoteAccessHandle = new $serviceClass;
    $this->remoteAccessHandle->setConfig(array("base_url" => $attrs['fdAccessServiceURL'][0],"username" => $attrs['fdAccessServiceUser'][0], "password" => $attrs['fdAccessServicePassword'][0]));
  }

  function pushconfig(){
    $this->createAccessConnection();
    $jsonAppData = $this->remoteAccessHandle->generateConfig($this->generateAppData());
    $this->remoteAccessHandle->updateApplication($this->fdAccessApplicationAuthType, $jsonAppData);
  }

  function deleteconfig(){
    $this->createAccessConnection();
    $this->remoteAccessHandle->deleteApplication($this->fdAccessApplicationAuthType,$this->parent->getBaseObject()->cn);
  }


  function array_diff_recursive($arr1, $arr2)
  {
    #thanks : https://stackoverflow.com/questions/3876435/recursive-array-diff
      $outputDiff = [];
      foreach ($arr1 as $key => $value)
      {
          //if the key exists in the second array, recursively call this function 
          //if it is an array, otherwise check if the value is in arr2
          if (array_key_exists($key, $arr2))
          {
              if (is_array($value))
              {
                  $recursiveDiff = $this->array_diff_recursive($value, $arr2[$key]);
  
                  if (count($recursiveDiff))
                  {
                      $outputDiff[$key] = $recursiveDiff;
                  }
              }
              else if (!in_array($value, $arr2))
              {
                  $outputDiff[$key] = $value;
              }
          }
          //if the key is not in the second array, check if the value is in 
          //the second array (this is a quirk of how array_diff works)
          else if (!in_array($value, $arr2))
          {
              $outputDiff[$key] = $value;
          }
      }
  
      return $outputDiff;
  }


  function compareconfig(){
    $this->createAccessConnection();
    $remoteconfig=$this->remoteAccessHandle->getApplication($this->fdAccessApplicationAuthType,$this->parent->getBaseObject()->cn)['data'];
    $localconfig = json_decode($this->remoteAccessHandle->generateConfig($this->generateAppData()),true);
    var_dump($this->array_diff_recursive($remoteconfig,$localconfig));
  }



  function importconfig(){
    $this->createAccessConnection();
    $importedConfig=$this->remoteAccessHandle->importApplication($this->fdAccessApplicationAuthType,$this->parent->getBaseObject()->cn);
    // fixme import function should only import inside the UI not directly Save it
    $this->fdAccessApplicationMacros=$importedConfig['macros'];
    $this->save();
  }

  function generateAppData(){
      global $config;
      $ldap = $config->get_ldap_link();

      $application=array();

      // fetch attributs
      foreach($this->fdAccessApplicationAttributes as $attribute_infos) {
        $attribute_data=preg_split("/;/",$attribute_infos);
        $attribute_dn=$attribute_data[0];
        $attribute_mandatory=$attribute_data[1];
        $attribute_scope=$attribute_data[2];
        $attribute_samlformat=$attribute_data[3];
        
        $ldap->cd($attribute_dn);
        $ldap->search('(objectClass=fdAccessAttributes)', ['cn','fdAccessAttributeOID','fdAccessAttributeAlias']);

        while( $attr_data = $ldap->fetch()) {
          $application['attributes'][$attr_data['cn'][0]]=array(
            "friendlyName" => $attr_data['fdAccessAttributeAlias'][0],
            "mandatory" => $attribute_mandatory,
            "scope" => $attribute_scope,
            "format" => $attribute_samlformat,
            "name" => $attr_data['fdAccessAttributeOID'][0]
        );
        }


      }

      //fetch app name
      $application['appname']=$this->parent->getBaseObject()->cn;
      //fetch auth type
      $application['type']=$this->fdAccessApplicationAuthType;
      // fetch macros

      $application['macros'] = $this->fdAccessApplicationMacros;
      $application['options'] = $this->fdAccessApplicationSettings;

      if ( isset($this->fdAccessApplicationSAMLMetadataFile)){
        $application['metadata'] = $this->fdAccessApplicationSAMLMetadataFile;
      }
      if ( isset($this->fdAccessApplicationScopeRules)){
        $application['scopeRules'] = $this->fdAccessApplicationScopeRules;
      }

/*
      foreach($this->fdAccessApplicationMacros as $macro_infos) {
        $array_macro=preg_split("/;/", $macro_infos);
        $application['macros'][$array_macro[0]]=$array_macro[1];
      }
      // fetch options
      foreach($this->fdAccessApplicationSettings as $settings_infos) {
        $array_settings=preg_split("/;/", $settings_infos);

        FIXME : un setings peut être multi valué
        $application['options'][$array_settings[0]]=$array_settings[1];
      }
*/
      return($application);




  }






  static function getAttributesInfo (): array
  {
    global $config;
    return [

      'accessApplication' => [
        'name'  => _('Common component'),
        'attrs' => [
          new SelectAttribute(
            _('Authentification Type'), _('Type of Authentification'),
             'fdAccessApplicationAuthType', TRUE,['OIDC','SAML','CAS','HEADER'],'',['OIDC','SAML','CAS','HEADER']),
          new accessServerSelectAttribute(
              _('Authentification server'), _('Authentification server'),
              'fdAccessApplicationAuthServerDn'
              ),
          new FileAttribute(
              _('SAML Metadata File'), _('SAML Metadata File'),
              'fdAccessApplicationSAMLMetadataFile', FALSE
              ),
          new ButtonAttribute(_('Push configuration'), _('Push configuration'),
              'fdAccessApplicationPushConfiguration', _('Push'),'pushconfig'
          ),
          new ButtonAttribute(_('Check configuration'), _('Check configuration'),
              'fdAccessApplicationCheckConfiguration', _('Check'),'compareconfig'
          ),
          new ButtonAttribute(_('Import configuration'), _('Import configuration'),
              'fdAccessApplicationImportConfiguration', _('Import'),'importconfig'
          ),
          new OrderedArrayAttribute(
            new URLAttribute(
              _('Service URIS'), _('Service URIS'),
              'fdAccessApplicationService', TRUE
            ),
            // Order disabled, Default values, Edit enabled
            FALSE, [], TRUE
          ),        
        ],
      ],

      'Attributes' => [
        'name'  => _('Attributes / Scope Rules definitions'),
        'attrs' => [
        new OrderedArrayAttribute(
          new CharSeparatedCompositeAttribute(
            _("Attributes"),
            'fdAccessApplicationAttributes',
              [
                new ObjectSelectAttribute(
                    _('Attribut'), _('Attribut'),
                    'fdAccessApplicationAttributes_dn', FALSE,['attributes']
                    ),
                new BooleanAttribute(
                    _('optional'), _('options'),
                    'fdAccessApplicationAttributes_options', FALSE, FALSE
                    ),
                new StringAttribute(
                    _('OIDC scope'), _('OIDC scope'),
                    'fdAccessApplicationAttributes_scope', FALSE, ""
                    ), 

                new SelectAttribute(
                    _('SAML Format'), _('Format to distribute attribute'),
                    'fdAccessApplicationAttributes_samlFormat', FALSE, ['URI','Basic','unspecified'],'',['URI','Basic','unspecified']
                    ),
                ],
            ';',
            '',
            '',
            
          ),
          FALSE,[],TRUE),
          new OrderedArrayAttribute(
            new CharSeparatedCompositeAttribute(
              _("ScopeRule"),
              'fdAccessApplicationScopeRules',
                [
                  new StringAttribute(
                      _('Scope Key'), _('Scope scope'),
                      'fdAccessApplicationScopeRules_key', FALSE, ""
                      ), 
                      new StringAttribute(
                        _('Scope rules'), _('OIDC rules'),
                        'fdAccessApplicationScopeRules_value', FALSE, ""
                        ),   

                  ],
              ';',
              '',
              '',
              
            ),
            FALSE,[],TRUE),

        ],

      ],
      'specificMacros' => [
        'name'  => _('Specific Macros'),
        //'class' => ['fullwidth'],
        'attrs' => [
            new OrderedArrayAttribute(
              new CharSeparatedCompositeAttribute(
                _('Macros'),
                'fdAccessApplicationMacros',
                [
                  new StringAttribute('', '', 'fdAccessApplicationMacros_label', TRUE),
                  new StringAttribute('', '', 'fdAccessApplicationMacros_expr', TRUE)
                ],
                ';',
                '',
                '',
                _('Macros')
              ),
              // no order
              FALSE,
              [],
              // edit button
              TRUE
            )            
        
            
            ],
      ],

      'specificAccess' => [
        'name'  => _('Specific settings'),
        //'class' => ['fullwidth'],
        'attrs' => [ 
            new OrderedArrayAttribute(
              new CharSeparatedCompositeAttribute(
                _('All specific settings per application'),
                'fdAccessApplicationSettings',
                [
                  new SelectAttribute('', '', 'fdAccessApplicationSettings_var', TRUE),
                  new StringAttribute('', '', 'fdAccessApplicationSettings_value', TRUE)
                ],
                ';',
                '',
                '',
                _('Settings')
              ),
              // no order
              FALSE,
              [],
              // edit button
              TRUE
            ),            
            ],
      ],
    ];
  }


 /* public function render (): string
  {
    $smarty = get_smarty();
    $smarty->assign('fdAccessApplicationAuthType',$this->fdAccessApplicationAuthType);
    return parent::render();
  }*/

  public function typeChanged(){
    $this->fdAccessApplicationAuthType=$this->attributesAccess['fdAccessApplicationAuthType']->getValue();
    //$this->fdAccessApplicationScopeRules=$this->attributesAccess['fdAccessApplicationScopeRules']->getValue();

    
    if ($this->fdAccessApplicationAuthType == "SAML" ){
      $this->attributesAccess['fdAccessApplicationSAMLMetadataFile']->setVisible(TRUE);
    }else{
      $this->attributesAccess['fdAccessApplicationSAMLMetadataFile']->setVisible(FALSE);
    }
    
    if ($this->fdAccessApplicationAuthType == "OIDC" ){
      $this->attributesAccess['fdAccessApplicationScopeRules']->setVisible(TRUE);
    }else{
      $this->attributesAccess['fdAccessApplicationScopeRules']->setVisible(FALSE);
    }
  }



  function __construct ($dn = NULL, $object = NULL, $parent = NULL, $mainTab = FALSE)
  {
    parent::__construct($dn, $object, $parent, $mainTab);
  
    $this->typeAuthentification=array('OIDC','SAML','CAS','HEADER');
    //$this->attributesAccess['fdAccessApplicationAuthType']->setInLdap(FALSE);
    $this->attributesAccess['fdAccessApplicationAuthType']->setChoices($this->typeAuthentification);

    $this->typeChanged();
    $this->attributesAccess['fdAccessApplicationAuthType']->setSubmitForm('typeChanged');
  }


  protected function updateFieldsChoicesAndValues ()
  {
    global $config;
    $access_config_var=array();
    $access_config_display=array();
    $ldap = $config->get_ldap_link();
      
    $taxonomy_dn=$config->get_cfg_value('TaxonomyTermsRDN').", ".$config->current['BASE'];
    $accessref=$config->get_cfg_value('AccessConfigReferential');
   
    $ldap->cd($taxonomy_dn);
    $ldap->search('(&(objectClass=fdTaxonomyTerms)(fdTaxonomyReferentiel='.$accessref.')(fdTaxonomyName='.$this->fdAccessApplicationAuthType.'))', ['fdTaxonomyTermCode','fdTaxonomyTermLongLabel']);
    while ($access_config = $ldap->fetch()) {
      $access_config_var[]=$access_config['fdTaxonomyTermCode'][0];
      $access_config_display[]=$access_config['fdTaxonomyTermLongLabel'][0];
      
    }
    
    $this->attributesAccess['fdAccessApplicationSettings']->attribute->attributes[0]->setChoices($access_config_var,$access_config_display);

    /* penser a nettoyer les vieille valeeur en cas de changement de protocole */


  }
    /* Values */

  public function update (): bool
  {
    parent::update();
    $this->updateFieldsChoicesAndValues();
    return TRUE;
  }


  public function update2 (): bool
  {
    parent::update();

    return TRUE;
  }

  protected function prepare_save (): array
  {
    $errors = parent::prepare_save();
  
    if (!empty($errors)) {
      return $errors;
    }
    //$this->pushconfig();
    return $errors;
  }


  protected function prepare_remove ()
  {
    parent::prepare_remove();
    /* Remove tagged values as well */
    $this->deleteconfig();
  }



}
