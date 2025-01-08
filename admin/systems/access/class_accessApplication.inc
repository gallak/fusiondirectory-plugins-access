<?php
/*
  This code is part of FusionDirectory (http://www.fusiondirectory.org/)
  Copyright (C) 2016-2023  FusionDirectory


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

/* instanciation of CompositeAttribute in order to add check value features */
 class ssoOptionsCharSeparatedCompositeAttribute extends CompositeAttribute
 {
   function __construct ($label, $description, $ldapName, $acl = "")
   {


    parent::__construct(
       $description, $ldapName,
       [
         new SelectAttribute(
           _('options'), _('options'),
           'fdAccessApplicationSettings_var', TRUE,
           array(), '',
           array()
         ),
         new StringAttribute(
           _('Value'), _('value'),
           'fdAccessApplicationSettings_value', TRUE
         )
       ],
       '/^([^;]+);(.*)$/',
       '%s;%s',
       $acl, $label
     );
   }
 
  
   function check ()
   {
      $rule=$this->getParent()->getRemoteSSOMapping()[$this->attributes[0]->getValue()];

      if ( !$rule['range']){
        $resultTest = accessRemoteValidation::runTest($rule['validation'],$this->attributes[1]->getValue());
        $resultMessage= $rule['syntax'];
      }else{
        $resultTest  = accessRemoteValidation::runTest($rule['validation'],$this->attributes[1]->getValue(),$rule['range']);
        $resultMessage= $rule['syntax']." with value ".implode(",",$rule['range']);
      }

      if ( ! $resultTest ){
        return new SimplePluginCheckError(
          $this,
          htmlescape(sprintf("Error on %s attribute : value provided %s. Please see : %s",$this->attributes[0]->getValue(),$this->attributes[1]->getValue(),$resultMessage))
        );
        $this->attributes[1]->setValue($this->attributes[0]->getValue());
      }else{
        $this->attributes[1]->setValue($this->attributes[1]->getValue());
      }
   }
   
 }
 
 class ssoAttributesCompositeAttributes extends CharSeparatedCompositeAttribute
 {
  function __construct ($protocol, $label, $description, $ldapName, $acl = ""){

    $oidc_attributes=array(
      new ObjectSelectAttribute(
        _('Attribut'), _('Attribut'),
        'fdAccessApplicationAttributes_dn', FALSE,['attributes']
        ),
      new StringAttribute(
        _('Remote name'), _('OIDC scope'),
        'fdAccessApplicationAttributes_name', FALSE, ""
        ), 
      new StringAttribute(
        _('OIDC scope'), _('OIDC scope'),
        'fdAccessApplicationAttributes_scope', FALSE, ""
        ), 
  
      new SelectAttribute(
        _('Value type'), _('type of value'),
        'fdAccessApplicationAttributes_type', FALSE, ['boolean','integer','string'],'',['boolean','integer','string']
        ),
      new SelectAttribute(
        _('Table Distribution'), _('Distribute as table'),
        'fdAccessApplicationAttributes_table', FALSE, ['automatic','always','never'],'',['automatic','always','never']
        )
    );
  
    $saml_attributes=array(
      new ObjectSelectAttribute(
        _('Attribut'), _('Attribut'),
        'fdAccessApplicationAttributes_dn', FALSE,['attributes']
        ),
      new BooleanAttribute(
        _('optional'), _('options'),
        'fdAccessApplicationAttributes_mandatory', FALSE, FALSE
        ),
      new SelectAttribute(
        _('SAML Format'), _('Format to distribute attribute'),
        'fdAccessApplicationAttributes_samlFormat', FALSE, ['URI','Basic','unspecified'],'',['URI','Basic','unspecified']
        )
      );
  
    $cas_attributes=array(
      new ObjectSelectAttribute(
        _('Attribut'), _('Attribut'),
        'fdAccessApplicationAttributes_dn', FALSE,['attributes']
        ),
      new StringAttribute(
        _('Remote name'), _('OIDC scope'),
        'fdAccessApplicationAttributes_name', FALSE, ""
        )
    );

  if ( $protocol == "oidc" ){
    parent::__construct($description, $ldapName, $oidc_attributes,';','','');  
  }
  if ( $protocol == "saml" ){
    parent::__construct($description, $ldapName, $saml_attributes,';','','');
  }
  if ( $protocol == "cas" ){
    parent::__construct($description, $ldapName, $cas_attributes,';','','');
  }
  }
 }
 



class accessWebApplication extends simplePlugin
{

  protected $displayHeader  = TRUE;
  protected $loaded = FALSE;

  // handle contain remote Access Management Service
  private $remoteAccessHandle;
  private $applicationOptionsMapping=array();
  public

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

  /* return type of remote SSO system */
  public function getRemoteSSOType(){
    return($this->remoteAccessHandle->getMethodCode());
  }
  /* return Mapping array between values store on remote SSO system and FusionDirectory Storage method*/
  public function getRemoteSSOMapping(){
    return($this->applicationOptionsMapping);
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

  /* push config to remote SSO*/
  function pushconfig(){
    $this->createAccessConnection();
    $jsonAppData = $this->remoteAccessHandle->generateConfig($this->generateAppData()); 
    $this->remoteAccessHandle->updateApplication($this->fdAccessApplicationAuthType, $jsonAppData);
  }

  function deleteconfig(){
    $this->createAccessConnection();
    $this->remoteAccessHandle->deleteApplication($this->fdAccessApplicationAuthType,$this->parent->getBaseObject()->cn);
  }


  function array_diff_recursive($arr1, $arr2)  {
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
    // get Remote SSO configuration
    $remoteconfig=$this->remoteAccessHandle->getApplication($this->fdAccessApplicationAuthType,$this->fdAccessApplicationConfKey);
    //var_dump($remoteconfig['data']);
    
    var_dump($this->remoteAccessHandle->configRemoteToPivot($remoteconfig));
    
    // get Local FD SSO configuration
    //$localconfig = json_decode($this->remoteAccessHandle->generateConfig($this->generateAppData()),true);
    
    //var_dump($localconfig);
    //var_dump($this->array_diff_recursive($remoteconfig['data'],$localconfig));
    //var_dump($this->generateAppData());
    var_dump("Version LocalToPivot");
    var_dump($this->configLocalToPivot());
    
  }



  function importconfig(){
    $this->createAccessConnection();
    $importedConfig=$this->remoteAccessHandle->importApplication($this->fdAccessApplicationAuthType,$this->parent->getBaseObject()->cn);
    // fixme import function should only import inside the UI not directly Save it
    $this->fdAccessApplicationMacros=$importedConfig['macros'];
    $this->save();
  }

  // get Local Config to Pivot
  function configLocalToPivot(){
    global $config;
    $ldap = $config->get_ldap_link();

    $application=array();

    //Appname section
    $application['appname']=$this->fdAccessApplicationConfKey;
    // Common section
    // build description
    $ui = get_userinfo();
    $today = date("d / m / y h:i");
    $application['description'] = $this->parent->getBaseObject()->description."  / Modified by ".$ui->uid. " on ".$today;

    // Authentification Section
    //$protocol=$this->fdAccessApplicationAuthType;
    $application['protocol']=$this->fdAccessApplicationAuthType;

    // add service URI
    $application['settings']['service']=$this->fdAccessApplicationService;    

    if ( $this->fdAccessApplicationAuthType == "OIDC"){
      if (! empty($this->fdAccessApplicationOIDCClientID)){
        $application["settings"]['clientId']=$this->fdAccessApplicationOIDCClientID;
      }
      if (! empty($this->fdAccessApplicationOIDCClientSecret)){
        $application["settings"]['clientSecret']=$this->fdAccessApplicationOIDCClientSecret;
      }
    }

    if ( $this->fdAccessApplicationAuthType == "SAML"){
      if ( ! empty($this->fdAccessApplicationSAMLMetadataFile)){
        $application["settings"]['metadata'] = $this->fdAccessApplicationSAMLMetadataFile;
      }
      if ( ! empty($this->fdAccessEntityAuthority)){
        $application["settings"]['entityAuthority']=$this->fdAccessEntityAuthority;
      }      
    }
    
    
    // Attributes Section
    $attributesFields="fdAccessApplication".$this->fdAccessApplicationAuthType."Attributes";
    foreach($this->$attributesFields as $attribute_infos) {
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

    
    // fetch macros

    if ( ! empty($this->fdAccessApplicationMacros)){
      $application['macros'] = $this->fdAccessApplicationMacros;
    }

    if (! empty($this->fdAccessApplicationSettings)){
      $application['additionnalSettings'] = $this->fdAccessApplicationSettings;
    }
    return($application);

  }

  // get data form FD record and store it an associative array before translating it to remote Access server syntax ( pivot format)
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
      // build description
      $ui = get_userinfo();
      $today = date("d / m / y h:i");
      $application['description'] = $this->parent->getBaseObject()->description."  / Modified by ".$ui->uid. " on ".$today;

      //fetch app name
      $application['appname']=$this->fdAccessApplicationConfKey;
      //fetch auth type
      $application['auth']=array('type' => $this->fdAccessApplicationAuthType);
      // fetch macros

      if ( ! empty($this->fdAccessApplicationMacros)){
        $application['macros'] = $this->fdAccessApplicationMacros;
      }

      if (! empty($this->fdAccessApplicationSettings)){
        $application['settings'] = $this->fdAccessApplicationSettings;
      }

      if (! empty($this->fdAccessApplicationOIDCClientID)){
        $application['auth']['clientId']=$this->fdAccessApplicationOIDCClientID;
      }
      if (! empty($this->fdAccessApplicationOIDCClientSecret)){
        $application['auth']['clientSecret']=$this->fdAccessApplicationOIDCClientSecret;
      }
      
      if ( ! empty($this->fdAccessApplicationSAMLMetadataFile)){
        $application['auth']['metadata'] = $this->fdAccessApplicationSAMLMetadataFile;
      }

      if ( ! empty($this->fdAccessEntityAuthority)){
        $application['auth']['entityAuthority']=$this->fdAccessEntityAuthority;
      }

      // add service URI
      $application['auth']['service']=$this->fdAccessApplicationService;
//var_dump($application);
      return($application);




  }


  static function getAttributesInfo (): array
  {
    global $config;
    return [

      'accessApplication' => [
        'name'  => _('Common component'),
        'attrs' => [

          new StringAttribute(
            _('Configuration Key'), _('Config Key, if not set cn is taken'),
            'fdAccessApplicationConfKey', TRUE),
          new SelectAttribute(
            _('Authentification Type'), _('Type of Authentification'),
             'fdAccessApplicationAuthType', TRUE,['OIDC','SAML','CAS'],'',['OIDC','SAML','CAS']),
          new SelectAttribute(
              _('type'), _('Type'),
              'fdAccessEntityType', FALSE,["IDP","SP"],"IDP",["IDP","SP"]),
          new StringAttribute(
              _('Federation'), _('Url of Federation if SP has been imported from a SAML Federation'),
              'fdAccessEntityAuthority', FALSE),
          new accessServerSelectAttribute(
              _('Authentification server'), _('Authentification server'),
              'fdAccessApplicationAuthServerDn'
              ),
          // dedicated OIDC
          new StringAttribute(
                _('Client ID'), _('Client ID'),
                'fdAccessApplicationOIDCClientID', FALSE),
          // dedicated OIDC
          new StringAttribute(
                  _('Client Secret'), _('Client Secret'),
                  'fdAccessApplicationOIDCClientSecret', FALSE),
          // dedicated SAML
          new StringAttribute(
              _('Organization'), _('Organization'),
              'o', FALSE),
          // dedicated SAML
          new StringAttribute(
              _('Technical Contact'), _('Technical Contact'),
              'mail', FALSE),
          // dedicated SAML
          new FileTextAreaAttribute(
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
              'fdAccessApplicationService', FALSE
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
            new ssoAttributesCompositeAttributes(
              "oidc",
              _("Attributes"),
              _("une description quelconque"),
              'fdAccessApplicationOIDCAttributes'
            ),FALSE,[],TRUE),
        
          new OrderedArrayAttribute(
            new ssoAttributesCompositeAttributes(
              "saml",
              _("Attributes"),
              _("une description quelconque"),
              'fdAccessApplicationSAMLAttributes'
            ),FALSE,[],TRUE),

          new OrderedArrayAttribute(
            new ssoAttributesCompositeAttributes(
              "cas",
              _("Attributes"),
              _("une description quelconque"),
              'fdAccessApplicationCASAttributes'
            ),FALSE,[],TRUE),
            
/*          new CharSeparatedCompositeAttribute(
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
            
          ),*/
          //FALSE,[],TRUE),
/*          new OrderedArrayAttribute(
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
*/
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
          //$label, $description, $ldapName, $acl = "",$remote)
            new OrderedArrayAttribute(
              new ssoOptionsCharSeparatedCompositeAttribute(
              //new CharSeparatedCompositeAttribute(
                _('Settings'),
                _('All specific settings per application'),
                'fdAccessApplicationSettings',
                ''
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



  public function typeChanged(){

    $this->fdAccessApplicationAuthType=$this->attributesAccess['fdAccessApplicationAuthType']->getValue();
    //$this->attributesAccess['fdAccessApplicationCASAttributes']->setVisible(FALSE);
    //$this->attributesAccess['fdAccessApplicationSAMLAttributes']->setVisible(FALSE);
    //$this->attributesAccess['fdAccessApplicationOIDCAttributes']->setVisible(FALSE);
    
    if ($this->fdAccessApplicationAuthType == "SAML" ){
      $this->attributesAccess['fdAccessApplicationSAMLMetadataFile']->setVisible(TRUE);
      $this->attributesAccess['fdAccessEntityAuthority']->setVisible(TRUE);
      $this->attributesAccess['o']->setVisible(TRUE);
      $this->attributesAccess['mail']->setVisible(TRUE);
      $this->attributesAccess['fdAccessApplicationService']->setVisible(FALSE);
      $this->attributesAccess['fdAccessEntityType']->setVisible(TRUE);
      $this->attributesAccess['fdAccessApplicationOIDCClientID']->setVisible(FALSE);
      $this->attributesAccess['fdAccessApplicationOIDCClientSecret']->setVisible(FALSE);
      $this->attributesAccess['fdAccessApplicationSAMLAttributes']->setVisible(TRUE);
      $this->attributesAccess['fdAccessApplicationCASAttributes']->setVisible(FALSE);
      $this->attributesAccess['fdAccessApplicationOIDCAttributes']->setVisible(FALSE);

    }else{
      // not an SAML type
      $this->attributesAccess['fdAccessApplicationSAMLMetadataFile']->setVisible(FALSE);
      $this->attributesAccess['fdAccessApplicationService']->setVisible(TRUE);

      $this->attributesAccess['fdAccessEntityAuthority']->setVisible(FALSE);
      $this->attributesAccess['o']->setVisible(FALSE);
      $this->attributesAccess['mail']->setVisible(FALSE);
      $this->attributesAccess['fdAccessEntityType']->setVisible(FALSE);
      $this->attributesAccess['fdAccessEntityType']->setValue(null);

  
    }
    
    if ($this->fdAccessApplicationAuthType == "OIDC" ){
      //$this->attributesAccess['fdAccessApplicationScopeRules']->setVisible(TRUE);
      $this->attributesAccess['fdAccessApplicationService']->setVisible(TRUE);
      $this->attributesAccess['fdAccessEntityType']->setVisible(FALSE);
      $this->attributesAccess['fdAccessApplicationOIDCClientID']->setVisible(TRUE);
      $this->attributesAccess['fdAccessApplicationOIDCClientSecret']->setVisible(TRUE);
      $this->attributesAccess['fdAccessApplicationSAMLMetadataFile']->setVisible(FALSE);
      $this->attributesAccess['fdAccessEntityAuthority']->setVisible(FALSE);
      $this->attributesAccess['o']->setVisible(FALSE);
      $this->attributesAccess['mail']->setVisible(FALSE);
      $this->attributesAccess['fdAccessApplicationSAMLAttributes']->setVisible(FALSE);
      $this->attributesAccess['fdAccessApplicationCASAttributes']->setVisible(FALSE);
      $this->attributesAccess['fdAccessApplicationOIDCAttributes']->setVisible(TRUE);
    }else{
      // not a OIDC type
      //$this->attributesAccess['fdAccessApplicationScopeRules']->setVisible(FALSE);
      $this->attributesAccess['fdAccessApplicationOIDCClientID']->setVisible(FALSE);
      $this->attributesAccess['fdAccessApplicationOIDCClientSecret']->setVisible(FALSE);
  
    }

    if ($this->fdAccessApplicationAuthType == "CAS" ){
      $this->attributesAccess['fdAccessApplicationSAMLAttributes']->setVisible(FALSE);
      $this->attributesAccess['fdAccessApplicationCASAttributes']->setVisible(TRUE);
      $this->attributesAccess['fdAccessApplicationOIDCAttributes']->setVisible(FALSE);
    }
  

    
    


  }

  public function serverChanged(){
    $this->fdAccessApplicationAuthServerDn = $this->attributesAccess['fdAccessApplicationAuthServerDn']->getValue();
    
    if ( empty($this->fdAccessApplicationAuthServerDn)) {
      //var_dump( $this->fdAccessApplicationAuthServerDn);
      $this->attributesAccess['fdAccessApplicationPushConfiguration']->setVisible(FALSE);
      $this->attributesAccess['fdAccessApplicationCheckConfiguration']->setVisible(FALSE);
      $this->attributesAccess['fdAccessApplicationImportConfiguration']->setVisible(FALSE);
    }else{
      $this->attributesAccess['fdAccessApplicationPushConfiguration']->setVisible(TRUE);
      $this->attributesAccess['fdAccessApplicationCheckConfiguration']->setVisible(TRUE);
      $this->attributesAccess['fdAccessApplicationImportConfiguration']->setVisible(TRUE);
    }
  }

  function __construct ($dn = NULL, $object = NULL, $parent = NULL, $mainTab = FALSE)
  {

    parent::__construct($dn, $object, $parent, $mainTab);
    
    //var_dump($this->attributesAccess);
                                      
    //var_dump($this->fdAccessApplicationAttributes);
                    
    $this->typeAuthentification=array('OIDC','SAML','CAS');
    $this->attributesAccess['fdAccessApplicationAuthType']->setChoices($this->typeAuthentification);
    $this->attributesAccess['fdAccessApplicationAuthServerDn']->setSubmitForm('serverChanged');
    $this->attributesAccess['fdAccessApplicationAuthType']->setSubmitForm('typeChanged');
    $this->typeChanged();
    $this->serverChanged();
    
    
  }


  protected function updateFieldsChoicesAndValues ()
  {
    global $config;
    $access_config_var=array();
    $access_config_display=array();
    $ldap = $config->get_ldap_link();

      if ( ! empty($this->fdAccessApplicationAuthServerDn)){
    $this->createAccessConnection();
    $this->applicationOptionsMapping=$this->remoteAccessHandle->getRemoteAccessServerMapping($this->fdAccessApplicationAuthType);
    // get only key and label for selectr option
    foreach($this->applicationOptionsMapping as $key => $value){
      $selectArray[$key]=$value['label'];
    }
    $this->attributesAccess['fdAccessApplicationSettings']->attribute->attributes[0]->setChoices(array_keys($selectArray),array_values($selectArray));  
    // check is option value should be flush in case of protocol modification
    // if first key isn't in option array, we flush it 
    // FIXME : when deleting one value all value are flush ....
    //if (! in_array(preg_split("/;/",$this->attributesAccess['fdAccessApplicationSettings']->getValue()[0])[0],array_keys($selectArray))){
    //  $this->attributesAccess['fdAccessApplicationSettings']->setValue(array());
    // }
  }
   
  }

  
  public function update (): bool
  {
    parent::update();
    $this->updateFieldsChoicesAndValues();
    return TRUE;
  }


  protected function prepare_save (): array
  {
    $errors = parent::prepare_save();
  
//mettre dans fdAccessApplicationAttributes  le contenu de fdAccessApplicationOIDCAttributes si OIDC  idem pour le chargemen


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
    if ( ! empty($this->fdAccessApplicationAuthServerDn)){
      $this->deleteconfig();
    }
    
  }



}
