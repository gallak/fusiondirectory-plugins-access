<?php

class accessRemoteKeyCloak extends accessRemoteCommon {

    private $cnx=array();
    private $uri="/api/v1";
    private $headers = array('Accept' => 'application/json', 'Content-Type' => 'application/json');
    private $param = array();

    private $currentMapping=array();

    private $mappinKC =array(
        "COMMON" => array(
                "fdAuthLevel"           => array("remotesso" =>  "KCAuthnLevel"),
                ),
        "OIDC" => array(
                "fdOIDCClientID"        => array("remotesso" =>  "KCOIDCClientID"),
                "fdOIDCClientSecret"    => array("remotesso" =>  "KCOIDCClientSecret"),
                "fdOIDCLogoutType"      => array("remotesso" =>  "KCOIDCLogoutType"),
                "fdOIDCRedirectURIs"    => array("remotesso" =>  "KCOIDCRedirectUris"),
                ),
        "SAML" => array(
                "fdSAMLIDPInitiated"    => array("remotesso" =>  "KCSAMLEnableIDPInitiatedURL"),
                "fdSAMLSignSLO"         => array("remotesso" =>  "KCSAMLSignSLOMessage"),
                "fdSAMLSignSSO"         => array("remotesso" =>  "KCSAMLSignSSOMessage"),
                "fdSAMLSignatureMethod" => array("remotesso" =>  "KCSAMLSignatureMethod"),
        ),

    );


    public function getOptionsMapping($protocol){
        $this->updateMappingFD($this->mappingKC);
        return($this->mappingFD[$protocol]);
    }

    public function __construct($cnx) {
        $this->cnx = $cnx;
    }

}
