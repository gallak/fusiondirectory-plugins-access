<?php




/* small php classe dedicated to test value */
class accessRemoteValidation{

    static function runTest($rule,$value){
        return($rule($value));  
    }
    
    private function is_bool($val){
        return(true);
    }
    
    private function is_integer($val){
        return(true);
    }
    
    private function is_string($val){
        var_dump($val);
        return(true);
    }
    
    private function is_uri($val){
        return(true);
    }
}


class accessRemoteCommon {

    private $cnx=array();
    private $uri="";
    private $headers = array('Accept' => 'application/json', 'Content-Type' => 'application/json');
    private $param = array();

    private $currentMapping=array();

    /* define main mapping of Fusiondirectory for Remote SSO system, should be update when class is instancied
      - remotesso : name of generic variable
      - validation : rule for validate entered data
      - label : text shown in drop list    
    */
    
    public $mappingFD =array(
        "COMMON" => array(),
        "CAS" => array(
                "fdCASAuthLevel"           => array("remotesso" =>  "Remote_CAS_Auth_Level_option", "validation" => "is_int", "label" => "Authentification Level"),
                "fdCASLogoutURL"        => array("remotesso" =>  "Remote_CAS_Logout", "validation" => "is_uri", "label" => "Logout URL"),
                "fdCASRule"             => array("remotesso" =>  "Remote_CAS_Rule", "validation" => "is_string", "label" => "Activation Rule"),
                "fdCASURLService"       => array("remotesso" =>  "Remote_CAS_Urls", "validation" => "is_uri", "label" => "Services Url(s)"),
                "fdCASUserAttribute"    => array("remotesso" =>  "Remote_CAS_User_attribute", "validation" => "is_string", "label" => "key used for user Id"),
                ),
        "OIDC" => array(
                "fdOIDCClientID"        => array("remotesso" =>  "Remote_OIDC_ClientID", "validation" => "is_string", "label" => "Client ID"),
                "fdOIDCClientSecret"    => array("remotesso" =>  "Remote_OIDC_ClientSecret", "validation" => "is_string", "label" => "Client Seccret"),
                "fdOIDCLogoutType"      => array("remotesso" =>  "Remote_OIDC_LogoutType", "validation" => "is_string", "label" => "LogoutType"),
                "fdOIDCRedirectURIs"    => array("remotesso" =>  "Remote_OIDC_RedirectUris", "validation" => "is_uri", "label" => "Redirect URI"),
                ),
        "SAML" => array(
                "fdSAMLIDPInitiated"    => array("remotesso" =>  "Remote_SAML_EnableIDPInitiatedURL", "validation" => "is_bool", "label" => "InitiatedIDP"),
                "fdSAMLSignSLO"         => array("remotesso" =>  "Remote_SAML_SignSLOMessage", "validation" => "is_bool", "label" => "Signing Single Log out"),
                "fdSAMLSignSSO"         => array("remotesso" =>  "Remote_SAML_SignSSOMessage", "validation" => "is_bool", "label" => "Signing Single Sign On"),
                "fdSAMLSignatureMethod" => array("remotesso" =>  "Remote_SAML_SignatureMethod", "validation" => "is_uri", "label" => "Signature Method"),
        ),

    );


    static function list_accessRemoteMethod(){


        global $class_mapping;
        $available = [];
        foreach ($class_mapping as $class => $path) {
          if (preg_match('/^accessRemote(.+)/', $class, $m)) {
            //$available[$m[1]] = $class::getName();
            $available[]=$class;
          }
        }

        return($available);
    }

    // improve FD mapping with real remote parameters
    public function updateMappingFD ($localMapping) {
        foreach ( $localMapping as $pk => $pv){
            foreach ( $pv as $vk => $vv){
                foreach($vv as $k => $v) {
                    $this->mappingFD[$pk][$vk][$k]=$v;
                }
             }
        }
    }



    public function getValidation($attr){
        foreach(self::$mappingFD as $protocol){
            foreach($protocol as $k => $v){
                if ( $k == $attr){
                    return($v[$k]['validation']);
                }
            }
        }

    }

    



    public function __construct($cnx) {
        $this->cnx = $cnx;
    }

    public function checkStatus(){
        $status="All's is OK, should remplaced by a real test ;)";
	    return($result);
	}

   

}
