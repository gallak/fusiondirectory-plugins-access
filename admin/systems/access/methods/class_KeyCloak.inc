<?php

class KeyCloak {

    private $cnx=array();
    private $uri="/api/v1";
    private $headers = array('Accept' => 'application/json', 'Content-Type' => 'application/json');
    private $param = array();

    private $currentMapping=array();

    private $mappingFD =array(
        "COMMON" => array(
                "fdAuthLevel"           => array("remotesso" =>  "KCAuthnLevel", "validation" => "is_int", "label" => "KC Authentification Level"),
                ),
        "OIDC" => array(
                "fdOIDCClientID"        => array("remotesso" =>  "KCOIDCClientID", "validation" => "is_string", "label" => "KC Client ID"),
                "fdOIDCClientSecret"    => array("remotesso" =>  "KCOIDCClientSecret", "validation" => "is_strning", "label" => "KC Client Seccret"),
                "fdOIDCLogoutType"      => array("remotesso" =>  "KCOIDCLogoutType", "validation" => "is_string", "label" => "KC LogoutType"),
                "fdOIDCRedirectURIs"    => array("remotesso" =>  "KCOIDCRedirectUris", "validation" => "is_uri", "label" => "KC Redirect URI"),
                ),
        "SAML" => array(
                "fdSAMLIDPInitiated"    => array("remotesso" =>  "KCSAMLEnableIDPInitiatedURL", "validation" => "is_bool", "label" => "KC InitiatedIDP"),
                "fdSAMLSignSLO"         => array("remotesso" =>  "KCSAMLSignSLOMessage", "validation" => "is_bool", "label" => "KC Signing Single Log out"),
                "fdSAMLSignSSO"         => array("remotesso" =>  "KCSAMLSignSSOMessage", "validation" => "is_bool", "label" => "KC Signing Single Sign On"),
                "fdSAMLSignatureMethod" => array("remotesso" =>  "KCSAMLSignatureMethod", "validation" => "is_uri", "label" => "KC Signature Method"),
        ),

    );

    private function is_bool($val){
        return(true);
    }

    private function is_integer($val){
        return(true);
    }

    private function is_string($val){
        return(true);
    }

    private function is_uri($val){
        return(true);
    }


    public function getOptionsMapping($protocole){
        return(array_merge($this->mappingFD[$protocole], $this->mappingFD['COMMON']));
    }

    public function getMappingSSOAttribute($type,$fdAttr){
        $mergeArray=array_merge($this->mappingFD[$type], $this->mappingFD['COMMON']);
        return($mergeArray[$fdAttr]['remotesso']);
    }

    public function getMappingSSOLabel($type,$fdAttr){
        $mergeArray=array_merge($this->mappingFD[$type], $this->mappingFD['COMMON']);
        return($mergeArray[$fdAttr][$fdAtrr]['label']);
    }

    public function validateMappingSSOValue($type,$fdAttr){
        $mergeArray=array_merge($this->mappingFD[$type], $this->mappingFD['COMMON']);
        $validateFocntion=$this->mappingFD[$type][$fdAttr]['validation'];
    }

    public function __construct($cnx) {
        $this->cnx = $cnx;
    }

}
