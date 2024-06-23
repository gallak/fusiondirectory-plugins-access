<?php
$url="main-all-renater-metadata.xml";
$str = file_get_contents($url);


 $xml=simplexml_load_string($str) or die("Error: Cannot create object");
 $xml->registerXPathNamespace('md',"urn:oasis:names:tc:SAML:2.0:metadata");
 $xml->registerXPathNamespace('mdrpi',"urn:oasis:names:tc:SAML:metadata:rpi");



 foreach ($xml->xpath("//md:EntityDescriptor") as $entity ) {
	if ($entity->children("md",true)->SPSSODescriptor) {
		print("Entite : ".$entity->attributes()['entityID']->__toString()."\n");
		print("Autority : ".$entity->children("md",true)->Extensions->children("mdrpi",true)->attributes()["registrationAuthority"]->__toString()."\n");
		print("Contact : ".$entity->children("md",true)->ContactPerson->EmailAddress->__toString()."\n");

		//print("Consumer :".$entity->children("md",true)->AssertionConsumerService->__toString()."\n");
//		var_dump($entity->children("md",true)->SPSSODescriptor->AssertionConsumerService->__toString()."\n");

		// Service URL :
		$location=$entity->children("md",true)->SPSSODescriptor->AssertionConsumerService->attributes()['Location']->__toString();
		$url=parse_url($location);
		print("service URL : ".$url['scheme']."://".$url['host']."\n");

		//  get ALl attributes
/*
		$attributes=$entity->children("md",true)->SPSSODescriptor->AttributeConsumingServiceAssertionConsumerService->attributes()['Location']->__toString();

        <md:RequestedAttribute NameFormat="urn:oasis:names:tc:SAML:2.0:attrname-format:uri" Name="urn:oid:1.3.6.1.4.1.5923.1.1.1.1" FriendlyName="eduPersonAffiliation"/>
        <md:RequestedAttribute NameFormat="urn:oasis:names:tc:SAML:2.0:attrname-format:uri" Name="urn:oid:1.3.6.1.4.1.5923.1.1.1.10" FriendlyName="eduPersonTargetedID" isRequired="true"/>
        <md:RequestedAttribute NameFormat="urn:oasis:names:tc:SAML:2.0:attrname-format:uri" Name="urn:oid:1.3.6.1.4.1.5923.1.1.1.6" FriendlyName="eduPersonPrincipalName"/>
        <md:RequestedAttribute NameFormat="urn:oasis:names:tc:SAML:2.0:attrname-format:uri" Name="urn:oid:2.16.840.1.113730.3.1.241" FriendlyName="displayName"/>
        <md:RequestedAttribute NameFormat="urn:oasis:names:tc:SAML:2.0:attrname-format:uri" Name="urn:oid:2.5.4.4" FriendlyName="sn"/>
        <md:RequestedAttribute NameFormat="urn:oasis:names:tc:SAML:2.0:attrname-format:uri" Name="urn:oid:2.5.4.42" FriendlyName="givenName"/>

*/
//		var_dump($entity->children("md",true)->SPSSODescriptor->AttributeConsumingService->RequestedAttribute);

//foreach ($entity->children("md",true)->SPSSODescriptor->AttributeConsumingService->RequestedAttribute as  $attr){
//	var_dump($attr->attributes()->{'Name'});
//}
		$attrArray=array();
		foreach ($entity->children("md",true)->SPSSODescriptor->AttributeConsumingService->RequestedAttribute as  $attr){
			$key=$attr->attributes()->{'FriendlyName'};
			//var_dump($key->__toString());
			print("key : ".$key."\n");
//			array_push($attrArray,array($key => array());
//			array_push($attrArray[$key], array("FriendlyName" => $attr->attributes()->{'FriendlyName'}));
			print("   - FriendlyName : ".$attr->attributes()->{'FriendlyName'}."\n");
                        print("   - NameFormat : ".$attr->attributes()->{'NameFormat'}."\n");
                        print("   - Name : ".$attr->attributes()->{'Name'}."\n");
                        print("   - isRequired : ".$attr->attributes()->{'isRequired'}."\n");

//			$attrArray[$key]['FriendlyName']=$attr->attributes()->{'FriendlyName'};
			//$attrArray[$key]['NameFormat']=$attr->attributes()->{'NameFormat'}->__toString();
			//$attrArray[$key]['Name']=$attr->attributes()->{'Name'}->__toString();
			//if (! $attr['isRequired']) {
			//	$attrArray[$key]['isRequired']=$attr->attributes()->{'isRequired'}->__toString();
			//}else{
			//	$attrArray[$key]['isRequired']="false";
			//}
//			print("Attributes : ".$attr->attributes()['Name']."\n");
			//var_dump($attr->attributes());
//			var_dump($attrArray);
		}
//*/
		//print("service URL : ".$entity->children("md",true)->SPSSODescriptor->AssertionConsumerService->attributes()['Location']->__toString()."\n");
//		var_dump($entity->children("md",true)->SPSSODescriptor->AssertionConsumerService->attributes()['Location']->__toString());
//<md:AssertionConsumerService Binding="urn:oasis:names:tc:SAML:2.0:bindings:HTTP-POST" Location="http://c2i2e.parisdescartes.fr/Shibboleth.sso/SAML2/POST" index="1"/>

print("======================\n");
	}

//      $arrayEntities[$this->getEntityID()]=array(
//        'fdAccessEntityAuthority' => $this->getRegistrationAuthority(),
//        'fdAccessEntityType' => $this->getSAMLType(),
//        'o' => $this->getOrgName(),
//        'mail' => $this->getOrgContactTechnical(),
//        'cn' => $this->getEntityID()
        //'metadata' => $this->getMetadata()
//      );
}

?>
