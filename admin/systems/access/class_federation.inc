<?php
/*
  This code is part of FusionDirectory (http://www.fusiondirectory.org/)
  Copyright (C) 2016-2017  FusionDirectory
  Copyright (C) 2021  Antoine Gallavardin

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


 /* federation entities are web application defined inside SAML federation as SP : Service Provider or IDP Identity Provider
  those applications are fecth from SAML Federation and import as webApplication object with Acces tab */
class federationEntities {
 
  public $xml;
  public $entity;
   
  function __construct($url) {
    $str = file_get_contents($url);
    $this->xml=simplexml_load_string($str) or die("Error: Cannot create object");
    $this->xml->registerXPathNamespace('md',"urn:oasis:names:tc:SAML:2.0:metadata");
    $this->xml->registerXPathNamespace('mdrpi',"urn:oasis:names:tc:SAML:metadata:rpi");
  }

  function getMetadataAuthority(){
    return($this->xml->attributes()['Name']->__toString());
  }
 
  function getOrgName(){
    return($this->entity->children("md",true)->Organization->OrganizationName->__toString());
  }
 
  function getEntityID(){
    return($this->entity->attributes()['entityID']->__toString());
  }
 
  function getRegistrationAuthority(){
    return($this->entity->children("md",true)->Extensions->children("mdrpi",true)->attributes()["registrationAuthority"]->__toString());
  }
 
  function getOrgContactTechnical(){
    return($this->entity->children("md",true)->ContactPerson->EmailAddress->__toString());
  }
 
  function getSAMLType(){
    if ( ! empty($this->entity->children("md",true)->SPSSODescriptor)) {
      return("SP");
    }elseif (! empty($this->entity->children("md",true)->IDPSSODescriptor)) {
      return("IDP");
    }else{
      return("N/A");
    }
  }
 
  function getMetadata(){
    return($this->entity->asXML());
  }
 
  function getUrlService(){
    if ($this->getSAMLType() == "SP" ){
       $location=$this->entity->children("md",true)->SPSSODescriptor->Extensions->children("mdui",true)->UIInfo->InformationURL->__toString();
     }else{
       $location=$this->entity->children("md",true)->IDPSSODescriptor->SingleSignOnService->attributes()->{'Location'}->__toString();
     }
    return($location);
  }
 
  function getTitle(){
    if ($this->getSAMLType() == "SP" ){
      $title=$this->entity->children("md",true)->SPSSODescriptor->Extensions->children("mdui",true)->UIInfo->DisplayName->__toString();
     }else{
      $title=$this->entity->children("md",true)->IDPSSODescriptor->Extensions->children("mdui",true)->UIInfo->DisplayName->__toString();
    }
    return($title);
  }
 
 
  function getDescription(){
    if ($this->getSAMLType() == "SP" ){
      $desc=$this->entity->children("md",true)->SPSSODescriptor->Extensions->children("mdui",true)->UIInfo->Description->__toString();
    }else{
      $desc=$this->entity->children("md",true)->IDPSSODescriptor->Extensions->children("mdui",true)->UIInfo->Description->__toString();
     }
    return($desc);
  }
 
 
 
 
  function getAttributes(){
    $attrArray=array();
    if ($this->getSAMLType() == "SP" ){
      if ( ! empty($this->entity->children("md",true)->SPSSODescriptor->AttributeConsumingService)){
        foreach ($this->entity->children("md",true)->SPSSODescriptor->AttributeConsumingService->RequestedAttribute as  $attr){
          if (! empty($attr->attributes()->{'isRequired'})){
            $mandatory=$attr->attributes()->{'isRequired'}->__toString();
          }else{
            $mandatory="false";
          }

        $attrArray[]=$attr->attributes()->{'FriendlyName'}->__toString().";".$mandatory.";;";

        }
      }
    }
    return($attrArray);
  }


  function createNameService($entityId){
  
    $entityUrlArray=parse_url($entityId);
    $entityUrlHost=str_replace(".","-",$entityUrlArray['host']);
    $entityUrlPath=str_replace("/","-",$entityUrlArray['path']);
    return($entityUrlHost.$entityUrlPath);

  }
 
  function getAllEntities(){
    $arrayEntities=array();
    foreach ($this->xml->xpath("//md:EntityDescriptor") as $entity ) {
      $this->entity = $entity;
        
        $attrObj=array(
          'objectClass' => array("fdWebApplication","fdAccessApplication"),
          'fdAccessEntityAuthority' => $this->getRegistrationAuthority(),
          'fdAccessEntityType' => $this->getSAMLType(),
           'o' => $this->getOrgName(),
           'mail' => $this->getOrgContactTechnical(),
           'cn' => $this->createNameService($this->getEntityID()),
           'fdAccessApplicationSAMLMetadataFile' => $this->getMetadata(),
           'labeledURI' => $this->getUrlService(),
           'fdApplicationTitle' => $this->getTitle(),
           'fdAccessApplicationAuthType' => "SAML",
           'fdAccessApplicationAuthServerDn' => "",
           'description' => $this->getDescription(),
           'fdAccessApplicationService' => $this->getUrlService(),
          );

          $samlAttr=$this->getAttributes();
          if (! empty($samlAttr)){
            $attrObj['fdAccessApplicationAttributes'] = $samlAttr;
          }
        $arrayEntities[$this->getEntityID()]=$attrObj;
    }
    return($arrayEntities);
  }
 
  function getOneEntitie($entity){
     return([$entity => $this->getAllEntities()[$entity]]);
  }
 
}
 






 
class federation extends simplePlugin
{

  protected $displayHeader  = TRUE;
  protected $loaded = FALSE;


  static function getFederationContent (): array 
  {
    return(array());
  }


  static function cleanAllEntities($federationURL){
    global $config;
    $ldap = $config->get_ldap_link();
    $entities_dn=$config->get_cfg_value('WebappsRDN').','.$config->current['BASE'];
    $ldap->cd($entities_dn);
    $ldap->search('(&(objectClass=fdAccessApplication)(fdAccessEntityAuthority='.$federationURL.'))', ['dn']);
    while ($rec = $ldap->fetch()) {
      $ldap->rmdir($rec['dn']);
    }
  }


  static function plInfo (): array
  {
    return [
      'plShortName'   => _('federation'),
      'plDescription' => _('federation'),
      'plObjectClass' => ['fdAccessFederation'],
      'plObjectType'  => ['federation' => [
        'name'        => _('federation'),
        'icon'        => 'geticon.php?context=applications&icon=federation&size=16',
        'ou'          => get_ou('AccessFederationRDN'),
        'mainAttr'    => 'cn',
      ]],
      'plSearchAttrs'  => ['description'],

      'plProvidedAcls'  => parent::generatePlProvidedAcls(static::getAttributesInfo())
    ];
  }

  function refreshMD(){
    global $config;

    $ldap = $config->get_ldap_link();
    $federationObject = new federationEntities($this->fdFederationURL);
    //  clean all local entities
    $this->cleanAllEntities($federationObject->getMetadataAuthority());
    // fetch all remote entities
    $allEntitiesInfos=$federationObject->getAllEntities();
    // insert into ldap
    foreach( $allEntitiesInfos as $entity => $content){
      if ( $entity == "https://apps.univ-lr.fr/idp/shibboleth" || $entity == "https://archive.espe-nice-toulon.fr" ){
        $dn = 'cn='.$content['cn'].','.$config->get_cfg_value('WebappsRDN').','.$config->current['BASE'];
        $ldap->cd($dn);
        $ldap->add($content);
        if (!$ldap->success()) {
          $error = new SimplePluginLdapError($this, $dn, LDAP_ADD, $ldap->get_error(), $ldap->get_errno());
          $error->display();
        }
      }
    }
  }




  static function getAttributesInfo (): array
  {
    return [
      'federation' => [
        'name'  => _('Information'),
        'attrs' => [
                new StringAttribute(
                  _('Name'), _('Name of Fedration'),
                  'cn', TRUE),
                new StringAttribute(
                  _('Metadata URL'), _('Url of Metadata'),
                  'fdFederationURL', TRUE),
                new StringAttribute(
                  _('Description'), _('Description'),
                  'description', FALSE),                
                new ButtonAttribute(_('Refresh Metadata'), _('Refresh Metadata'),
                  'fdFederationUpdateButton', _('Refresh'),'refreshMD'
                  ),
        ]
      ],
      'content' => [
        'name'  => _('Content of Federation'),
        'attrs' => [
          new FakeAttribute('fdFederationContent'),
          ],
        //'template' => get_template_path('datacenter.tpl', TRUE, dirname(__FILE__)),
        ],
    ];
  }


  function __construct ($dn = NULL, $object = NULL, $parent = NULL, $mainTab = FALSE)
  {
    global $config;
    parent::__construct($dn, $object, $parent, $mainTab);
    $this->fdFederationContentn=$this->getFederationContent();

  }




  function compute_dn (): string
  {
    global $config;
    return ("cn=".$this->cn.",".get_ou('AccessFederationRDN').$config->current['BASE']);
  }


}
