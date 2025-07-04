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
//       '/^([^;]+);(.*)$/',
       '/^(.*);(.*)$/',       
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
 
 
 

 class oidcCompositeAttributes extends CharSeparatedCompositeAttribute
 {
  function __construct ( $label, $description, $ldapName, $acl = ""){

    $attributes=array(
      new ObjectSelectAttribute(
        _('Attribut'), _('Attribut'),
        'fdAccessApplicationAttributes_dn', FALSE,['attributes']
        ),
///      new StringAttribute(
///        _('Remote name'), _('OIDC scope'),
///        'fdAccessApplicationAttributes_name', FALSE, ""
///        ), 
//      new StringAttribute(
//        _('OIDC scope'), _('OIDC scope'),
//        'fdAccessApplicationAttributes_scope', FALSE, ""
//        ), 
  
      new SelectAttribute(
        _('Value type'), _('type of value'),
        'fdAccessApplicationAttributes_type', FALSE, ['bool','int','string'],'',['boolean','integer','string']
        ),
      new SelectAttribute(
        _('Table Distribution'), _('Distribute as table'),
        'fdAccessApplicationAttributes_table', FALSE, ['automatic','always','never'],'',['automatic','always','never']
        )
    );
    parent::__construct($description, $ldapName, $attributes,';','','');  

  }
}

class oidcScopeCompositeAttributes extends CharSeparatedCompositeAttribute
{
 function __construct ( $label, $description, $ldapName, $acl = ""){

   $attributes=array(
    new StringAttribute(
      _('Scope name'), _('OIDC scope'),
      'fdOIDCScope_name', FALSE, ""
      ), 
    new StringAttribute(
       _('Attributs'), _('Attributs'),
       'fdOIDCScope_dn', FALSE,""
       ),
     new StringAttribute(
       _('OIDC rules'), _('OIDC scope rules'),
       'fdOIDCScope_rule', FALSE, ""
       ), 
   );
   parent::__construct($description, $ldapName, $attributes,';','','');  

 }
}

class samlCompositeAttributes extends CharSeparatedCompositeAttribute
{
  function __construct ( $label, $description, $ldapName, $acl = ""){
    $attributes=array(
      new ObjectSelectAttribute(
        _('Attribut'), _('Attribut'),
        'fdAccessApplicationAttributes_dn', FALSE,['attributes']
        ),
      new BooleanAttribute(
        _('Mandatory'), _('options'),
        'fdAccessApplicationAttributes_mandatory', FALSE, FALSE
        ),
      new SelectAttribute(
        _('SAML Format'), _('Format to distribute attribute'),
        'fdAccessApplicationAttributes_samlFormat', FALSE, ['urn:oasis:names:tc:SAML:2.0:attrname-format:uri','urn:oasis:names:tc:SAML:2.0:attrname-format:basic','urn:oasis:names:tc:SAML:2.0:attrname-format:unspecified'],'',['URI','Basic','unspecified']
        )
      );
      parent::__construct($description, $ldapName, $attributes,';','',''); 
    }
}

class casCompositeAttributes extends CharSeparatedCompositeAttribute
{
  function __construct ( $label, $description, $ldapName, $acl = ""){
    $attributes=array(
      new ObjectSelectAttribute(
        _('Attribut'), _('Attribut'),
        'fdAccessApplicationAttributes_dn', FALSE,['attributes']
        ),
      new StringAttribute(
        _('Remote name'), _('Remote Name'),
        'fdAccessApplicationAttributes_name', FALSE, ""
        )
    );
      parent::__construct($description, $ldapName, $attributes,';','',''); 
    }
}  
  

class accessWebApplication extends simplePlugin
{

  protected $displayHeader  = TRUE;
  protected $loaded = FALSE;

  // handle contain remote Access Management Service
  private $remoteAccessHandle;
  private $applicationOptionsMapping=array();

  static function plInfo (): array
  {
    return [
      'plShortName'   => _('Access Web Application'),
      'plDescription' => _('Access Web Application'),
      'plPriority'    => 30,
      'plObjectClass' => ['fdAccessApplication'],
      'plObjectType'  => ['webApplication'],
      'plProvidedAcls'  => parent::generatePlProvidedAcls(static::getAttributesInfo()),
      'plIcon'        => 'geticon.php?context=applications&icon=access&size=48',
      'plSmallIcon'   => 'geticon.php?context=applications&icon=access&size=16',
      'plSelfModify'  => TRUE,
    ];
  }


  /* Get All Attributes referenced by FusionDirectory */
  public function getAvailableAttributesbyDN(){
    global $config;
    $scope = "sub";
    $ldapEntries = objects::ls("attributes", ['cn'=> 1, "description" => 1, "fdAccessAttributeClaims" => 1,"fdAccessAttributeOID" => 1,"fdAccessAttributeFriendlyName" => 1],$config->current['BASE'], '(objectClass=*)', TRUE, $scope, FALSE, TRUE);
    return($ldapEntries);
  }

  public function getAvailableAttributesbyCN(){
    $ldapEntries = $this->getAvailableAttributesbyDN();
    $attributesArray=array();
    foreach ($ldapEntries as $dn => $values){
      $attributesArray[$ldapEntries[$dn]['cn']] = $values;
      $attributesArray[$ldapEntries[$dn]['cn']]['dn'] = $dn;
    }
    return($attributesArray);
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
    $ldap->search('(objectClass=fdAccessService)', ['fdAccessServiceType','fdAccessServiceURL','fdAccessServiceUser','fdAccessServicePassword','fdAccessServiceReqCert']);
    $attrs = $ldap->fetch();
    $serviceClass="accessMethod".$attrs['fdAccessServiceType'][0];
    $this->remoteAccessHandle = new $serviceClass;

    // set curl SSL options
    if ( $attrs['fdAccessServiceReqCert'][0] == "TRUE" ){
      $curl_options=array(CURLOPT_SSL_VERIFYPEER => 1 ,CURLOPT_SSL_VERIFYHOST => 2 );
    }else{
      $curl_options=array(CURLOPT_SSL_VERIFYPEER => 0 ,CURLOPT_SSL_VERIFYHOST => 0 );
    }
    $this->remoteAccessHandle->setConfig(array("base_url" => $attrs['fdAccessServiceURL'][0],"username" => $attrs['fdAccessServiceUser'][0], "password" => $attrs['fdAccessServicePassword'][0], "curl_options" => $curl_options));
  }

  /* push config to remote SSO*/
  function pushconfig(){
    $this->createAccessConnection();
    $localPivotConfiguration=$this->configLocalToPivot();
    $jsonAppData = $this->remoteAccessHandle->configPivotToRemote($localPivotConfiguration,$this->getAvailableAttributesbyCN());
    $this->remoteAccessHandle->updateApplication($this->fdAccessApplicationAuthType, $jsonAppData);
  }

  function deleteconfig(){
    $this->createAccessConnection();
    $this->remoteAccessHandle->deleteApplication($this->fdAccessApplicationAuthType,$this->parent->getBaseObject()->cn);
  }


// set recursive array as flat array, key is the path of final value
  private function flatten($record, $out,$currentkey)	{
    foreach($record as $k => $v){
      if (is_array($v)){
        if ( $currentkey == ""){
          $chemin=$k;
        }else{
          $chemin=$currentkey." > ".$k;
        }
        $out=$this->flatten($v,$out,$chemin);
      }else{
        
        if ( $currentkey == ""){
          $out[$k]=$v;
        }else{
          $out[$currentkey." > ".$k]=$v;
        }		
      }
    }
    return($out);
  }



  function diff_config($local,$remote){
    $AllOut=array();
    $diff_config=array();
    
    // set recursive array as flat array, key is the path of final value
    $flat_local=$this->flatten($local,$AllOut,"");
    $flat_remote=$this->flatten($remote,$AllOut,"");
    
    // make diff in twice sens for getting all differences  
    $diff_complete=array_merge(array_diff($flat_local,$flat_remote),array_diff($flat_remote,$flat_local));
    // walk across all difference
    foreach($diff_complete as $k => $v){
      if (! isset($flat_local[$k])){
        $local_value="N/A";
      }else{
        $local_value=$flat_local[$k];
      }
      $diff_config[$k]["local"]=$local_value;
      if (! isset($flat_remote[$k])){
        $remote_value="N/A";
      }else{
        $remote_value=$flat_remote[$k];
      }
      $diff_config[$k]["remote"]=$remote_value;
    }
    return $diff_config;
    }


  function compareconfig(){
    
    
    $this->createAccessConnection();
    // get Remote SSO configuration
    $remoteconfig=$this->remoteAccessHandle->getApplication($this->fdAccessApplicationAuthType,$this->fdAccessApplicationConfKey);
    
    $remotePivot=$this->remoteAccessHandle->configRemoteToPivot($this->fdAccessApplicationAuthType,$remoteconfig,$this->getAvailableAttributesbyCN());
    $localPivot=$this->configLocalToPivot();

    $diffConf=$this->diff_config($localPivot,$remotePivot);
    if ( $diffConf){
      $diffTable="<table width='100%' border='1' border-collapse='collapse'><tr><th>"._("Value")."</th><th>"._("Local value")."</th><th>"._("Remote value")."</th><tr>";
      foreach($this->diff_config($localPivot,$remotePivot) as $k => $v){
        $diffTable=$diffTable."<tr><td>".$k."</td><td>".$v["local"]."</td><td>".$v["remote"]."</td><tr>";
      }
      $diffTable=$diffTable."</table>"; 
    }else{
      $diffTable=_("No differences found for supported options");
    }
    msg_dialog::display(_("Comparaison"), $diffTable,INFO_DIALOG);
  }

  /* import config from remote Configuration after pivot */
  function importconfig(){
    $this->createAccessConnection();
    // get Remote SSO configuration
    $remoteconfig=$this->remoteAccessHandle->getApplication($this->fdAccessApplicationAuthType,$this->fdAccessApplicationConfKey);
    $remotePivot=$this->remoteAccessHandle->configRemoteToPivot($this->fdAccessApplicationAuthType,$remoteconfig,$this->getAvailableAttributesbyCN());
        
    
    // Per Protocol
    if ($remotePivot["protocol"] == "OIDC"){
      $this->attributesAccess['fdAccessApplicationService']->setValue($remotePivot["settings"]["service"]);
      $this->attributesAccess['fdAccessApplicationOIDCClientID']->setValue($remotePivot["settings"]["clientId"]);
      $this->attributesAccess['fdAccessApplicationOIDCClientSecret']->setValue($remotePivot["settings"]["clientSecret"]);
      foreach ($remotePivot["attributes"] as $attrs => $values){  
        $lattributsArray[]=$remotePivot["attributes"][$attrs]["dn"].";".$remotePivot["attributes"][$attrs]['format'].";".$remotePivot["attributes"][$attrs]['table'];
      }
      $this->attributesAccess['fdAccessApplicationOIDCAttributes']->setValue($lattributsArray);

      //scope info import
      if ($remotePivot["scope"]){
        $lscopeArray=array();
        foreach ($remotePivot["scope"] as $pivot_k => $pivot_v){  
          $lscope=$pivot_k.";".$pivot_v['attributs'];
          if ($pivot_v['rules']){
            $lscope=$lscope.";".$pivot_v['rules'];
          }
          $lscopeArray[]=$lscope;
        }
        $this->attributesAccess['fdAccessApplicationOIDCScopes']->setValue($lscopeArray);
      }
    }
    if ($remotePivot["protocol"] == "SAML"){
      $this->attributesAccess['fdAccessApplicationSAMLMetadataFile']->setValue($remotePivot["settings"]["metadata"]);
      $this->attributesAccess['fdAccessEntityType']->setValue("SP");
      
      foreach ($remotePivot["attributes"] as $attrs => $values){  
        $lattributsArray[]=$remotePivot["attributes"][$attrs]["dn"].";".$remotePivot["attributes"][$attrs]['mandatory'].";".$remotePivot["attributes"][$attrs]['format'];
      }
      $this->attributesAccess['fdAccessApplicationSAMLAttributes']->setValue($lattributsArray);

    }

    if ($remotePivot["protocol"] == "CAS"){
      $this->attributesAccess['fdAccessApplicationService']->setValue($remotePivot["settings"]["service"]);
      foreach ($remotePivot["attributes"] as $attrs => $values){  
        $lattributsArray[]=$remotePivot["attributes"][$attrs]["dn"].";".$remotePivot["attributes"][$attrs]['remote_variable'];
      }
      $this->attributesAccess['fdAccessApplicationCASAttributes']->setValue($lattributsArray);
    }

    if ($remotePivot["additionnalSettings"]){
    foreach ($this->getRemoteSSOMapping() as $fdAttr => $fdValue){
      foreach ($remotePivot["additionnalSettings"] as $remoteAttr => $remoteValue){
          if ($fdAttr == $remoteAttr ){
              $local_optionnalSettings[]=$fdAttr.";".$remoteValue;
          }
      }
    }
    $this->attributesAccess['fdAccessApplicationSettings']->setValue($local_optionnalSettings);
    }
    
    // Macros Management
    if ($remotePivot["macros"]){
      foreach($remotePivot["macros"] as $k => $v){
        $lmacros[]=$k.";".$v;
      }
      $this->attributesAccess['fdAccessApplicationMacros']->setValue($lmacros);
    }
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
    // Authentification Section
    $application['protocol']=$this->fdAccessApplicationAuthType;

    if ( $this->fdAccessApplicationAuthType == "OIDC"){
      // add service URI
      $application['settings']['service']=$this->fdAccessApplicationService;    

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
    // get Attributes Field name
    $attributesFieldName="fdAccessApplication".$this->fdAccessApplicationAuthType."Attributes";

    // get All available Attributes;

    $availableAttributes = $this->getAvailableAttributesbyDN();
    foreach($this->$attributesFieldName as $attribute_infos) {
      // collect current attribute config
      $attribute_data=preg_split("/;/",$attribute_infos);
      // get identifier of attribut 
      $attr_id=$availableAttributes[$attribute_data[0]]['cn'];
      // common values
      $application['attributes'][$attr_id] = array(
        "dn" => $attribute_data[0]
      );
      
     
      if ( $application['protocol'] == "OIDC" ){
        $application['attributes'][$attr_id]["format"]=$attribute_data[1];
        $application['attributes'][$attr_id]["table"]=$attribute_data[2];
      }

      if ( $application['protocol'] == "SAML" ){
        $application['attributes'][$attr_id]["format"]=$attribute_data[2];
        $application['attributes'][$attr_id]["mandatory"]=$attribute_data[1];
      }

      if ( $application['protocol'] == "CAS" ){
          $application['attributes'][$attr_id]["remote_variable"]=$attribute_data[1];
          // add service URI
          $application['settings']['service']=$this->fdAccessApplicationService;    
        }
      // Scope management
      if ( $application['protocol'] == "OIDC" ){
        foreach($this->fdAccessApplicationOIDCScopes as $scope_infos){
          $scope_data=preg_split("/;/",$scope_infos);
          $scope_name=$scope_data[0];
          $application['scope'][$scope_name]['attributs']=$scope_data[1];
          $application['scope'][$scope_name]['rules']=$scope_data[2];
        }
      }
      // fetch macros
      if ( ! empty($this->fdAccessApplicationMacros)){
        foreach ($this->fdAccessApplicationMacros as $k => $v){
          $v_array=explode(";",$v);
          $application['macros'][$v_array[0]] = $v_array[1];
        }
      }

      if (! empty($this->fdAccessApplicationSettings)){
        foreach ($this->fdAccessApplicationSettings as $value){
          $valueArray = explode(";", $value);
          $application['additionnalSettings'][$valueArray[0]] =$valueArray[1];
        }
      
      }
    }
    return($application);
    
  }


  public function generateClientSecret () {
    global $config;
    $result = bin2hex(random_bytes(50));
    $this->fdAccessApplicationOIDCClientSecret = $result;
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
          new ButtonAttribute(_('Generate Client Secret'), _('Generate Secret'),
                  'fdAccessApplicationOIDCGenSecret', _('Generate'),'generateClientSecret'
              ),
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
            new oidcCompositeAttributes(
              _("Attributes"),
              _("Attributes OIDC"),
              'fdAccessApplicationOIDCAttributes'
            ),FALSE,[],TRUE),
            
            new OrderedArrayAttribute(
              new oidcScopeCompositeAttributes(
                _("Scope"),
                _("OIDC Scope"),
                'fdAccessApplicationOIDCScopes'
              ),FALSE,[],TRUE),

          new OrderedArrayAttribute(
            new samlCompositeAttributes(
              _("Attributes"),
              _("Attributes SAML"),
              'fdAccessApplicationSAMLAttributes'
            ),FALSE,[],TRUE),

          new OrderedArrayAttribute(
            new casCompositeAttributes(
              _("Attributes"),
              _("Attributes CAS"),
              'fdAccessApplicationCASAttributes'
            ),FALSE,[],TRUE),
            
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
      $this->attributesAccess['fdAccessApplicationOIDCScopes']->setVisible(FALSE);
      $this->attributesAccess['fdAccessApplicationOIDCGenSecret']->setVisible(FALSE);

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
      $this->attributesAccess['fdAccessApplicationOIDCScopes']->setVisible(TRUE);
      $this->attributesAccess['fdAccessApplicationOIDCGenSecret']->setVisible(TRUE);
    }else{
      // not a OIDC type
      //$this->attributesAccess['fdAccessApplicationScopeRules']->setVisible(FALSE);
      $this->attributesAccess['fdAccessApplicationOIDCClientID']->setVisible(FALSE);
      $this->attributesAccess['fdAccessApplicationOIDCClientSecret']->setVisible(FALSE);
      $this->attributesAccess['fdAccessApplicationOIDCGenSecret']->setVisible(FALSE);
      $this->attributesAccess['fdAccessApplicationService']->setVisible(FALSE);
    }

    if ($this->fdAccessApplicationAuthType == "CAS" ){
      $this->attributesAccess['fdAccessApplicationService']->setVisible(TRUE);
      $this->attributesAccess['fdAccessApplicationSAMLAttributes']->setVisible(FALSE);
      $this->attributesAccess['fdAccessApplicationCASAttributes']->setVisible(TRUE);
      $this->attributesAccess['fdAccessApplicationOIDCAttributes']->setVisible(FALSE);
      $this->attributesAccess['fdAccessApplicationOIDCScopes']->setVisible(FALSE);
    }
  

    
    


  }

  public function serverChanged(){
    $this->fdAccessApplicationAuthServerDn = $this->attributesAccess['fdAccessApplicationAuthServerDn']->getValue();
    
    if ( empty($this->fdAccessApplicationAuthServerDn)) {
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
    $selectArray[""]="";
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
