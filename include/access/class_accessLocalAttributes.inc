<?php

class accessLocalAttributes {


/* Get All Attributes referenced by FusionDirectory */
public function getAvailableLocalAttributesbyDN(){
    global $config;
    $scope = "sub";
    $ldapEntries = objects::ls("attributes", ['fdAccessAttributePivot'=> 1, 
                                              "description" => 1,
                                              "fdAccessAttributeSource" => 1,
                                              "fdAccessAttributeClaims" => 1,
                                              "fdAccessAttributeOID" => 1,
                                              "fdAccessAttributeFriendlyName" => 1
                                            ],$config->current['BASE'], '(objectClass=*)', TRUE, $scope, FALSE, TRUE);
    return($ldapEntries);
  }

  public function getAvailableLocalAttributesbyCN(){
    $ldapEntries = $this->getAvailableLocalAttributesbyDN();
    $attributesArray=array();
    foreach ($ldapEntries as $dn => $values){
      $attributesArray[$ldapEntries[$dn]['fdAccessAttributePivot']] = $values;
      $attributesArray[$ldapEntries[$dn]['fdAccessAttributePivot']]['dn'] = $dn;
    }
   
    return($attributesArray);
  }

  // ICI
  public function getAvailableLocalAttributesbyOID(){
    $ldapEntries = $this->getAvailableLocalAttributesbyDN();
    $attributesArray=array();
    foreach ($ldapEntries as $dn => $values){
      $attributesArray[$ldapEntries[$dn]['fdAccessAttributeOID']] = $values;
      $attributesArray[$ldapEntries[$dn]['fdAccessAttributeOID']]['dn'] = $dn;
    }
   
    return($attributesArray);
  }

}