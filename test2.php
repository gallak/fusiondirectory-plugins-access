<?php
class federationEntities {

  public $xml;
  public $entity;
  
  function __construct($url) {
    $str = file_get_contents($url);

    $this->xml=simplexml_load_string($str) or die("Error: Cannot create object");
    $this->xml->registerXPathNamespace('md',"urn:oasis:names:tc:SAML:2.0:metadata");
    $this->xml->registerXPathNamespace('mdrpi',"urn:oasis:names:tc:SAML:metadata:rpi");
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
       //$location=$this->entity->children("md",true)->SPSSODescriptor->AssertionConsumerService->attributes()['Location']->__toString();
       //$url=parse_url($location);
       //return($location);
    }else{
	$location=$this->entity->children("md",true)->IDPSSODescriptor->SingleSignOnService->attributes()->{'Location'}->__toString();
      //<md:SingleSignOnService Binding="urn:oasis:names:tc:SAML:2.0:bindings:HTTP-Redirect" Location="https://adfs.devinci.fr/adfs/ls/"/>

      //return("url IDP");
    }
return($location);
  }

  function getTitle(){
     if ($this->getSAMLType() == "SP" ){
       $title=$this->entity->children("md",true)->SPSSODescriptor->Extensions->children("mdui",true)->UIInfo->DisplayName->__toString();
       //return($title);
    }else{
	$title=$this->entity->children("md",true)->IDPSSODescriptor->Extensions->children("mdui",true)->UIInfo->DisplayName->__toString();
    }
    return($title);
  }


  function getDescription(){
     if ($this->getSAMLType() == "SP" ){
       $desc=$this->entity->children("md",true)->SPSSODescriptor->Extensions->children("mdui",true)->UIInfo->Description->__toString();
       //return($desc);
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
      //$key=$attr->attributes()->{'FriendlyName'};
      if (! empty($attr->attributes()->{'isRequired'})){
        $mandatory=$attr->attributes()->{'isRequired'}->__toString();
      }else{
        $mandatory="false";
      }
      $attrArray[]=array("FriendlyName" => $attr->attributes()->{'FriendlyName'}->__toString(),
//                         "NameFormat" => $attr->attributes()->{'NameFormat'}->__toString(),
//                         "Name" => $attr->attributes()->{'Name'}->__toString(),
                         "isRequired" => $mandatory
                   );
      }
}
//    return($key);
	}
    return($attrArray);
  
}

  function getAllEntities(){
    $arrayEntities=array();
    foreach ($this->xml->xpath("//md:EntityDescriptor") as $entity ) {
	$this->entity = $entity;
      $arrayEntities[$this->getEntityID()]=array(
        'fdAccessEntityAuthority' => $this->getRegistrationAuthority(),
        'fdAccessEntityType' => $this->getSAMLType(),
        'o' => $this->getOrgName(),
        'mail' => $this->getOrgContactTechnical(),
        'cn' => $this->getEntityID(),
        'metadata' => $this->getMetadata(),
        'labelelURI' => $this->getUrlService(),
        'fdApplicationTitle' => $this->getTitle(),
        'fdAccessApplicationAuthType' => "SAML",
        'fdAccessApplicationAttributes' => $this->getAttributes(),
        'fdAccessApplicationAuthServerDn' => "",
        'description' => $this->getDescription(),
        'fdAccessApplicationService' => $this->getUrlService()
      );
      $this->getDescription();
    }
    return($arrayEntities);

  }

  function getOneEntitie($entity){
    return([$entity => $this->getAllEntities()[$entity]]);
  }

}

    $entitiesList = new federationEntities("./main-all-renater-metadata.xml");
    $allInfos=$entitiesList->getAllEntities();

var_dump($allInfos);
?>
