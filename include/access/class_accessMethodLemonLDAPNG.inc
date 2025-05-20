<?php

//require_once("netdisco/restclient.php");


class accessMethodLemonLDAPNG {

    private $name="LemonLDAP::NG";
    private $code="LemonLDAPNG";
    private $version="0.1";
    private $handle="";
    private $serviceConfig=array();
    


    private $mapping = array(
        "CAS" => "casAppMetaData",
        "OIDC" => "oidcRPMetaData",
        "SAML" => "samlSPMetaData"
    );

    /* local mapping 
      - remotesso : name of LL::NG variable
      - validation : rule for validate entered data
      - label : text shown in drop list
      - range : values autorized or present un list
      for LL:NG :     #https://gitlab.ow2.org/lemonldap-ng/lemonldap-ng/-/blob/v2.0/lemonldap-ng-manager/lib/Lemonldap/NG/Manager/Build/Attributes.pm
    */

    private $mappingLLNG =array(
        "COMMON" => array(
            "fdCOMMONComment"        => array(
                "remotesso" =>  "comment",
                "syntax" => "It's the description",
                "label" => "Comment",
                "validation" => "is_string"
            ),
            "fdCOMMONAuthLevel"        => array(
                "remotesso"=>  "authnLevel",
                "syntax" => "It must an integer corresponding to auth Level see : https://lemonldap-ng.org",
                "range" => array("-3","5"),
                "validation" => "is_integer",          
                "label" => "Authentification level"
                ),
            "fdCOMMONRule"        => array(
                "remotesso" =>  "rule",
                "syntax" => "It 's the description",
                "label" => "Activation Rule",
                "validation" => "is_string"
                ),
            "fdCOMMONdisplayName" => array(
                "remotesso" =>  "displayName",
                "syntax" => "It must be a string",
                "validation" => "is_string",
                "label" => "Display  Name",
                "range" => array(""),
                ),
            ),
        "CAS" => array(
                "fdCASLogoutURL"        => array(
                        "remotesso" =>  "logout",
                        "syntax" => "It must a integer",
                        "label" => "Logout Url",
                        "range" => array("-1","1"),
                        "validation" => "is_integer"
                        ),
                "fdCASUserAttribute"    => array(
                        "remotesso" =>  "userAttribute",
                        "syntax" => "It must the name of attribute wich is send as username",
                        "label" => "User Attribute",
                        "validation" => "is_string"
                    ),
                "fdCASlogout" => array(
                    "remotesso" =>  "logout",
                    "syntax" => "It must be a string",
                    "validation" => "is_string",
                    "label" => "Logout Back-channel logout",
                    "range" => array(""),
                    ),
        
                ),
        "OIDC" => array(
                "fdOIDCLogoutType"      => array(
                        "remotesso" =>  "logoutType",
                        "label"         => "Type of logout",
                        "validation" => "is_string"
                        ),
                "fdOIDCServiceEncAlgo"  => array(
                        "remotesso" =>  "EncAlgorithmEnc", 
                        "range"      => array("A256CBC-HS512","A256GCM","A192CBC-HS384","A192GCM","A128CBC-HS256","A128GCM"),
                        "validation" => "is_select",                        
                        "label"         => "Encryption algorithm"
                        ),
                "fdOIDCServiceAccessTokenEncAlgo"  => array(
                        "remotesso" =>  "accessTokenEncContentEncAlg", 
                        "range"      => array("A256CBC-HS512","A256GCM","A192CBC-HS384","A192GCM","A128CBC-HS256","A128GCM"),
                        "validation" => "is_select",
                        "label" => "Token Encryption algorithm"
                        ),
                        
                "fdOIDCServiceIcon"  => array(
                        "remotesso" =>  "icon",
                        "validation" => "is_string",
                        "label" => "Path of icon of service"
                        ),
                "fdOIDCPublicClient"  => array(
                        "remotesso" =>  "public",
                        "validation" => "is_bool",
                        "label" => "OIDC Public Client"
                        ),
                        "fdOIDCaccessTokenClaims" => array(
                            "remotesso" =>  "accessTokenClaims",
                            "syntax" => "It must be a string",
                            "validation" => "is_string",
                            "label" => "Release claims in Access Token",
                            "range" => array(""),
                            ),
                        "fdOIDCaccessTokenEncContentEncAlg" => array(
                            "remotesso" =>  "accessTokenEncContentEncAlg",
                            "syntax" => "It must be a string",
                            "validation" => "is_select",
                            "label" => "Access Token Encryption content encryption algorithm",
                            "range" => array("A256CBC-HS512","A256GCM","A192CBC-HS384","A192GCM","A128CBC-HS256","A128GCM"),
                            ),
                        "fdOIDCaccessTokenEncKeyMgtAlg" => array(
                            "remotesso" =>  "accessTokenEncKeyMgtAlg",
                            "syntax" => "It must be a string",
                            "validation" => "is_select",
                            "label" => "Access Token Encryption key management alg",
                            "range" => array("A256CBC-HS512","A256GCM","A192CBC-HS384","A192GCM","A128CBC-HS256","A128GCM"),
                            ),
                        "fdOIDCaccessTokenExpiration" => array(
                            "remotesso" =>  "accessTokenExpiration",
                            "syntax" => "It must be a string",
                            "validation" => "is_string",
                            "label" => "Access Tokens Access Tokens",
                            "range" => array(""),
                            ),
                        "fdOIDCaccessTokenJWT" => array(
                            "remotesso" =>  "accessTokenJWT",
                            "syntax" => "It must be a string",
                            "validation" => "is_string",
                            "label" => "Use JWT format for Access Token",
                            "range" => array(""),
                            ),
                        "fdOIDCaccessTokenSignAlg" => array(
                            "remotesso" =>  "accessTokenSignAlg",
                            "syntax" => "It must be a string",
                            "validation" => "is_select",
                            "label" => "Access Token signature algorithm",
                            "range" => array("A256CBC-HS512","A256GCM","A192CBC-HS384","A192GCM","A128CBC-HS256","A128GCM"),
                            ),
                        "fdOIDCadditionalAudiences" => array(
                            "remotesso" =>  "additionalAudiences",
                            "syntax" => "It must be a string",
                            "validation" => "is_string",
                            "label" => "Additional audiences",
                            "range" => array(""),
                            ),
                        "fdOIDCallowClientCredentialsGrant" => array(
                            "remotesso" =>  "allowClientCredentialsGrant",
                            "syntax" => "It must be a boolean",
                            "validation" => "is_bool",
                            "label" => "Allow OAuth2.0 Client Credentials Grant",
                            "range" => array(""),
                            ),
                        "fdOIDCallowOffline" => array(
                            "remotesso" =>  "allowOffline",
                            "syntax" => "It must be a boolean",
                            "validation" => "is_bool",
                            "label" => "Allow offline access Allow offline access",
                            "range" => array(""),
                            ),
                        "fdOIDCallowPasswordGrant" => array(
                            "remotesso" =>  "allowPasswordGrant",
                            "syntax" => "It must be a string",
                            "validation" => "is_string",
                            "label" => "Allow OAuth2.0 Password Grant",
                            "range" => array(""),
                            ),
                        "fdOIDCauthMethod" => array(
                            "remotesso" =>  "authMethod",
                            "syntax" => "It must be a string",
                            "validation" => "is_string",
                            "label" => "Token endpoint authentication method",
                            "range" => array(""),
                            ),
                        "fdOIDCauthnEndpointAuthMethod" => array(
                            "remotesso" =>  "authnEndpointAuthMethod",
                            "syntax" => "It must be a string",
                            "validation" => "is_string",
                            "label" => "Authentication method for authorization code request",
                            "range" => array(""),
                            ),
                        "fdOIDCtokenEndpointAuthMethod" => array(
                            "remotesso" =>  "tokenEndpointAuthMethod",
                            "syntax" => "It must be a string",
                            "validation" => "is_string",
                            "label" => "Token endpoint authentication method",
                            "range" => array(""),
                            ),
                        "fdOIDCauthnRequireNonce" => array(
                            "remotesso" =>  "authnRequireNonce",
                            "syntax" => "It must be a string",
                            "validation" => "is_string",
                            "label" => "Require \nonce\ in authorization request",
                            "range" => array(""),
                            ),
                        "fdOIDCauthnRequireState" => array(
                            "remotesso" =>  "authnRequireState",
                            "syntax" => "It must be a string",
                            "validation" => "is_string",
                            "label" => "Require \state\ in authorization request",
                            "range" => array(""),
                            ),
                        "fdOIDCauthRequiredForAuthorize" => array(
                            "remotesso" =>  "authRequiredForAuthorize",
                            "syntax" => "It must be a string",
                            "validation" => "is_string",
                            "label" => "Require JWS for authorization requests",
                            "range" => array(""),
                            ),
                        "fdOIDCbypassConsent" => array(
                            "remotesso" =>  "bypassConsent",
                            "syntax" => "It must be a boolean",
                            "validation" => "is_bool",
                            "label" => "Bypass consent",
                            "range" => array(""),
                            ),
                        "fdOIDCextraClaims" => array(
                            "remotesso" =>  "extraClaims",
                            "syntax" => "It must be a string",
                            "validation" => "is_string",
                            "label" => "Scope values content Extra claims",
                            "range" => array(""),
                            ),
                        "fdOIDCidTokenEncContentEncAlg" => array(
                            "remotesso" =>  "idTokenEncContentEncAlg",
                            "syntax" => "It must be a string",
                            "validation" => "is_select",
                            "label" => "ID Token Encryption content encryption algorithm",
                            "range" => array("A256CBC-HS512","A256GCM","A192CBC-HS384","A192GCM","A128CBC-HS256","A128GCM"),
                            ),
                        "fdOIDCidTokenEncKeyMgtAlg" => array(
                            "remotesso" =>  "idTokenEncKeyMgtAlg",
                            "syntax" => "It must be a string",
                            "validation" => "is_select",
                            "label" => "ID Token Encryption key management alg",
                            "range" => array("A256CBC-HS512","A256GCM","A192CBC-HS384","A192GCM","A128CBC-HS256","A128GCM"),
                            ),
                        "fdOIDCiDTokenExpiration" => array(
                            "remotesso" =>  "iDTokenExpiration",
                            "syntax" => "It must be a string",
                            "validation" => "is_string",
                            "label" => "ID Tokens ID Tokens",
                            "range" => array(""),
                            ),
                        "fdOIDCiDTokenForceClaims" => array(
                            "remotesso" =>  "iDTokenForceClaims",
                            "syntax" => "It must be a string",
                            "validation" => "is_string",
                            "label" => "Force claims to be returned in ID Token",
                            "range" => array(""),
                            ),
                        "fdOIDCiDTokenSignAlg" => array(
                            "remotesso" =>  "iDTokenSignAlg",
                            "syntax" => "It must be a string",
                            "validation" => "is_string",
                            "label" => "ID Token signature algorithm",
                            "range" => array(""),
                            ),
                        "fdOIDClogoutBypassConfirm" => array(
                            "remotesso" =>  "logoutBypassConfirm",
                            "syntax" => "It must be a boolean",
                            "validation" => "is_bool",
                            "label" => "Bypass confirm",
                            "range" => array(""),
                            ),
                        "fdOIDClogoutEncContentEncAlg" => array(
                            "remotesso" =>  "logoutEncContentEncAlg",
                            "syntax" => "It must be a string",
                            "validation" => "is_select",
                            "label" => "Logout Encryption content encryption algorithm",
                            "range" => array("A256CBC-HS512","A256GCM","A192CBC-HS384","A192GCM","A128CBC-HS256","A128GCM"),
                            ),
                        "fdOIDClogoutEncKeyMgtAlg" => array(
                            "remotesso" =>  "logoutEncKeyMgtAlg",
                            "syntax" => "It must be a string",
                            "validation" => "is_select",
                            "label" => "Logout Encryption key management alg",
                            "range" => array("A256CBC-HS512","A256GCM","A192CBC-HS384","A192GCM","A128CBC-HS256","A128GCM"),
                            ),
                        "fdOIDClogoutSessionRequired" => array(
                            "remotesso" =>  "logoutSessionRequired",
                            "syntax" => "It must be a string",
                            "validation" => "is_string",
                            "label" => "Session required",
                            "range" => array(""),
                            ),
                        "fdOIDClogoutUrl" => array(
                            "remotesso" =>  "logoutUrl",
                            "syntax" => "It must be a string",
                            "validation" => "is_string",
                            "label" => "URL",
                            "range" => array(""),
                            ),
                        "fdOIDCpublic" => array(
                            "remotesso" =>  "public",
                            "syntax" => "It must be a boolean",
                            "validation" => "is_bool",
                            "label" => "Public client",
                            "range" => array(""),
                            ),
                        "fdOIDCredirectUris" => array(
                            "remotesso" =>  "redirectUris",
                            "syntax" => "It must be a string",
                            "validation" => "is_string",
                            "label" => "Allowed redirection addresses for login",
                            "range" => array(""),
                            ),
                        "fdOIDCpostLogoutRedirectUris" => array(
                            "remotesso" =>  "postLogoutRedirectUris",
                            "syntax" => "It must be a string",
                            "validation" => "is_string",
                            "label" => "Allowed redirection addresses for logout",
                            "range" => array(""),
                            ),
                        "fdOIDCrefreshToken" => array(
                            "remotesso" =>  "refreshToken",
                            "syntax" => "It must be a string",
                            "validation" => "is_string",
                            "label" => "Use refresh tokens",
                            "range" => array(""),
                            ),
                        "fdOIDCrefreshTokenRotation" => array(
                            "remotesso" =>  "refreshTokenRotation",
                            "syntax" => "It must be a string",
                            "validation" => "is_string",
                            "label" => "Invalidate refresh tokens after use",
                            "range" => array(""),
                            ),
                        "fdOIDCrequirePKCE" => array(
                            "remotesso" =>  "requirePKCE",
                            "syntax" => "It must be a boolean",
                            "validation" => "is_bool",
                            "label" => "Require PKCE",
                            "range" => array(""),
                            ),
                        "fdOIDCuserIDAttr" => array(
                            "remotesso" =>  "userIDAttr",
                            "syntax" => "It must be a string",
                            "validation" => "is_string",
                            "label" => "User attribute",
                            "range" => array(""),
                            ),
                        "fdOIDCuserInfoEncContentEncAlg" => array(
                            "remotesso" =>  "userInfoEncContentEncAlg",
                            "syntax" => "It must be a string",
                            "validation" => "is_select",
                            "label" => "Userinfo encryption content encryption algorithm",
                            "range" => array("A256CBC-HS512","A256GCM","A192CBC-HS384","A192GCM","A128CBC-HS256","A128GCM"),
                            ),
                        "fdOIDCuserInfoEncKeyMgtAlg" => array(
                            "remotesso" =>  "userInfoEncKeyMgtAlg",
                            "syntax" => "It must be a string",
                            "validation" => "is_select",
                            "label" => "Userinfo encryption key management algorithm",
                            "range" => array("A256CBC-HS512","A256GCM","A192CBC-HS384","A192GCM","A128CBC-HS256","A128GCM"),
                            ),
                        "fdOIDCuserinfoRequireHeaderToken" => array(
                            "remotesso" =>  "userinfoRequireHeaderToken",
                            "syntax" => "It must be a string",
                            "validation" => "is_string",
                            "label" => "Userinfo endpoint authentication using header only",
                            "range" => array(""),
                            ),
                        "fdOIDCuserInfoSignAlg" => array(
                            "remotesso" =>  "userInfoSignAlg",
                            "syntax" => "It must be a string",
                            "validation" => "is_string",
                            "label" => "Userinfo response format",
                            "range" => array(""),
                            ),
            
                ),
        "SAML" => array(
                "fdSAMLIDPInitiated"    => array(
                        "remotesso" =>  "enableIDPInitiatedURL",
                        "label" => "Enable IDP Initiated",
                        "syntax" => "It must be a boolean",
                        "validation" => "is_bool",
                        ),
                "fdSAMLSignSLO"         => array(
                        "remotesso" =>  "signSLOMessage",
                        "label" => "SLO Message signature",
                        "syntax" => "It must be a boolean",
                        "validation" => "is_string",
                        ),
                "fdSAMLSignSSO"         => array(
                        "remotesso" =>  "signSSOMessage",
                        "label" => "SSO Message signature",
                        "syntax" => "It must be a boolean",
                        "validation" => "is_string",
                        ),
                "fdSAMLSignatureMethod" => array(
                        "remotesso" =>  "signatureMethod",
                        "label" => "Signature Method",
                        "syntax" => "It must be a choice",
                        "validation" => "is_string",
                        ),
                "fdSAMLFederationEntityID" => array(
                        "remotesso" =>  "federationEntityID",
                        "syntax" => "It must be an URL",
                        "validation" => "is_string",
                        "label" => "SAML Federation attached"
                        ),
                "fdSAMLFederationOptionalAttributes" => array(
                        "remotesso" =>  "FederationOptionalAttributes",
                        "syntax" => "It must be an boolean",
                        "validation" => "is_string",
                        "label" => "Optionnal Attribtutes"
                        ),
                "fdSAMLFederationRequiredAttributes" => array(
                        "remotesso" =>  "FederationRequiredAttributes", 
                        "syntax" => "It must be an boolean",
                        "validation" => "is_string",
                        "label" => "Mandatory Attribtutes"
                        ),
                        "fdSAMLcheckSLOMessageSignature" => array(
                            "remotesso" =>  "checkSLOMessageSignature",
                            "syntax" => "It must be a boolean",
                            "validation" => "is_bool",
                            "label" => "Check SLO message signature Check SLO message signature",
                            "range" => array(""),
                            ),
                        "fdSAMLcheckSSOMessageSignature" => array(
                            "remotesso" =>  "checkSSOMessageSignature",
                            "syntax" => "It must be a boolean",
                            "validation" => "is_bool",
                            "label" => "Check SSO message signature Check SSO message signature",
                            "range" => array(""),
                            ),
                        "fdSAMLencryptionMode" => array(
                            "remotesso" =>  "encryptionMode",
                            "syntax" => "It must be a string",
                            "validation" => "is_string",
                            "label" => "Encryption mode Encryption mode",
                            "range" => array(""),
                            ),
                        "fdSAMLfederationRequiredAttributes" => array(
                            "remotesso" =>  "federationRequiredAttributes",
                            "syntax" => "It must be a string",
                            "validation" => "is_string",
                            "label" => "Required attributes",
                            "range" => array(""),
                            ),
                        "fdSAMLforceUTF8" => array(
                            "remotesso" =>  "forceUTF8",
                            "syntax" => "It must be a boolean",
                            "validation" => "is_bool",
                            "label" => "Force UTF-8 UTF8 metadata conversion Force UTF-8",
                            "range" => array(""),
                            ),
                        "fdSAMLnameIDFormat" => array(
                            "remotesso" =>  "nameIDFormat",
                            "syntax" => "It must be a string",
                            "validation" => "is_string",
                            "label" => "NameID format Default NameID format",
                            "range" => array(""),
                            ),
                        "fdSAMLnameIDSessionKey" => array(
                            "remotesso" =>  "nameIDSessionKey",
                            "syntax" => "It must be a string",
                            "validation" => "is_string",
                            "label" => "Force NameID session key",
                            "range" => array(""),
                            ),
                        "fdSAMLnotOnOrAfterTimeout" => array(
                            "remotesso" =>  "notOnOrAfterTimeout",
                            "syntax" => "It must be a string",
                            "validation" => "is_string",
                            "label" => "notOnOrAfter duration",
                            "range" => array(""),
                            ),
                        "fdSAMLoneTimeUse" => array(
                            "remotesso" =>  "oneTimeUse",
                            "syntax" => "It must be a string",
                            "validation" => "is_string",
                            "label" => "One time use",
                            "range" => array(""),
                            ),
                        "fdSAMLSessionNotOnOrAfterTimeout" => array(
                            "remotesso" =>  "SessionNotOnOrAfterTimeout",
                            "syntax" => "It must be a string",
                            "validation" => "is_string",
                            "label" => "sessionNotOnOrAfter duration",
                            "range" => array(""),
                            ),
                        "fdSAMLsignatureMethod" => array(
                            "remotesso" =>  "signatureMethod",
                            "syntax" => "It must be a string",
                            "validation" => "is_string",
                            "label" => "Signature method",
                            "range" => array(""),
                            ),
                    ),

    );




    // return all option per protocol
    public function getRemoteAccessServerMapping($protocol){
        
        return(array_merge($this->mappingLLNG[$protocol],$this->mappingLLNG['COMMON']));
    }

    public function getMethodCode(): string{
        return($this->code);
    }

    public function getMethodName(): string{
        return($this->name);
    }

    function setConfig($conf) {
        $this->serviceConfig=$conf;
    }


    //Revoir les construceurs et implication access Method et LemopnLDAP
    function checkStatus($conf) {
        $handle= new accessRemoteLemonLDAPNG($conf);
        return($handle->checkStatus());
    }

    function addApplication($type,$content){
        $addfct="add".ucfirst(strtolower($type))."App";
        $handle=new accessRemoteLemonLDAPNG($this->serviceConfig);
        $res=$handle->$addfct($content);
        return($res);
    }


    function replaceApplication($type,$content){
        $replacefct="replace".ucfirst(strtolower($type))."App";
        $handle=new accessRemoteLemonLDAPNG($this->serviceConfig);
        $res=$handle->$replacefct(json_decode($content)->confKey,$content);
        return($res);
    }

    function ifExistApplication($type,$appConfKey){

        $getfct="get".ucfirst(strtolower($type))."AppByConfKey";
        $handle=new accessRemoteLemonLDAPNG($this->serviceConfig);
        $res=$handle->$getfct($appConfKey);
        if ( $res['code'] == 200 ){
            return(true);
        }else{
            return(false);
        }
    
    }

    # Check if applications exist
    function checkApplication($type,$appConfKey){

        $getfct="get".ucfirst(strtolower($type))."AppByConfKey";
        $handle=new accessRemoteLemonLDAPNG($this->serviceConfig);
        $res=$handle->$getfct($appConfKey);
        
        if ( preg_match("/^4..$/",$res['code'])){
          msg_dialog::display(_($type." application error"), sprintf(_("An error occurs during saving application. <br> %s <br> Detailed error : <br> %s"),$res['msg'],$res['data']['error']),ERROR_DIALOG);
        }else{
          msg_dialog::display(_($type." application information"), sprintf(_("Application %s exists"),$appConfKey,),INFO_DIALOG);
        }
    
    }

    # Get Row information from Remote SSO systeme
    function getApplication($type,$appConfKey){

        $getfct="get".ucfirst(strtolower($type))."AppByConfKey";
        $handle=new accessRemoteLemonLDAPNG($this->serviceConfig);
        $res=$handle->$getfct($appConfKey);
        return($res);
    }

    /* remove empty value from remote config */
    function cleanRemoteConfig($remote){
        $config=$remote['data'];
       // clean phase of remote options config
        $duplicate_data=['redirectUris','clientId'];
        foreach ($duplicate_data as $attr){
            unset($config['options'][$attr]);
        }
        // clean empty value in each sections
        // maybe use array_walk_recursive ?
      

        foreach ($config as $k => $v){
            if (empty($v)){
                unset($config[$k]);
            }else{
                if ( is_array($config[$k])){
                    foreach ($config[$k] as $subk => $subv){
                        if (empty($subv)){
                            unset($config[$k][$subk]);
                        }
                    }
                }
            }
        }
        return($config);
    }

    // Transform remote LL::NG response to Pivot Format
    function configRemoteToPivot($type,$remoteConfig,$availableAttributes){

        $cleanConfig=$this->cleanRemoteConfig($remoteConfig);
        $remotePivot=array();
        // check Type of authentification

        $remotePivot['appname']=$cleanConfig['confKey'];
        $remotePivot['macros']=$cleanConfig['macros'];
        $remotePivot['protocol']=$type;


        // I'm a OIDC config
        if ($type == "OIDC"){
            // main setttings
            $remotePivot['settings']['service']=$cleanConfig['redirectUris'];
            $remotePivot['settings']['clientId']=$cleanConfig['clientId'];
            $remotePivot['settings']['clientSecret']=$cleanConfig['options']['clientSecret'];
            // attribute OIDC is matched to the Fusion Attribute List trough the Claim name
            foreach ($cleanConfig['exportedVars'] as $k => $v){
                $attributes_array=explode(";",$v);
                foreach ($availableAttributes as $attr => $info){
                    if ($info['fdAccessAttributeClaims'] == $k){
                        $remotePivot['attributes'][$attr]['dn']=$info['dn'];
                        if (isset($attributes_array[1])){
                            $remotePivot['attributes'][$attr]['format']=$attributes_array[1];
                        }else{
                            $remotePivot['attributes'][$attr]['format']="";
                        }
                        if (isset($attributes_array[2])){
                            $remotePivot['attributes'][$attr]['table']=$attributes_array[2];
                        }else{
                            $remotePivot['attributes'][$attr]['table']="";
                        }
                    break;
                    }
                }
            }
            
            foreach ($cleanConfig['extraClaims'] as $k => $v){
                $remotePivot['scope'][$k]['attributs']=$v;
            }

            foreach ($cleanConfig['scopeRules'] as $k => $v){
                $remotePivot['scope'][$k]['rules']=$v;
            }
                        
        }

        if ($type == "CAS"){
            foreach ($cleanConfig['exportedVars'] as $k => $v){
                $remotePivot['attributes'][$k]['dn']=$availableAttributes[$k]['dn'];
                $remotePivot['attributes'][$k]['remote_variable']=$v;
            }
            if ($cleanConfig['options']['service']){
                $remotePivot['settings']['service']=$cleanConfig['options']['service'];
            }
        }

        if ($type == "SAML"){
            // fetch Metadata
            $remotePivot['settings']['metadata'] = $cleanConfig['metadata'];


            // attribute SAML is matched to the Fusion Attribute List trough the name (OID)
            foreach ($cleanConfig['exportedAttributes'] as $k => $v){
                foreach ($availableAttributes as $attr => $info){
                    if ($info['fdAccessAttributeOID'] == $v['name']){
                        $remotePivot['attributes'][$attr]['dn']=$info['dn'];
                        if (isset($v['mandatory'])){
                            $remotePivot['attributes'][$attr]['mandatory']=$v['mandatory'];
                        }else{
                            $remotePivot['attributes'][$attr]['mandatory']="";
                        }
                        if (isset($v['format'])){
                            $remotePivot['attributes'][$attr]['format']=$v['format'];
                        }else{
                            $remotePivot['attributes'][$attr]['format']="";
                        }
                    break;
                    }
                }
            }            
        }


        foreach ($this->getRemoteAccessServerMapping($type) as $fdAttr => $fdValue){
            foreach ($cleanConfig['options'] as $remoteAttr => $remoteValue){
                if ($fdValue['remotesso'] == $remoteAttr ){
                    /*if (is_array($remoteValue)){
                        $value=implode("|",$remoteValue);
                    }else{
                        $value=$remoteValue;
                    }*/
                    $remotePivot['additionnalSettings'][$fdAttr] = $remoteValue;
                }
            }

        }


        return($remotePivot);
    
    }




    function importApplication($type,$appConfKey){

        $getfct="get".ucfirst(strtolower($type))."AppByConfKey";
        $handle=new accessRemoteLemonLDAPNG($this->serviceConfig);
        $res=$handle->$getfct($appConfKey);

     
        // import scopeRule
        foreach($res['data']['scopeRules'] as $key => $value){
            $arrayScopeRule[]=$key.";".$value;
        }
        // import macros
        foreach($res['data']['macros'] as $key => $value){
            $arrayMacros[]=$key.";".$value;
        }        
        
        $imported['macros']=$arrayMacros;
        return($imported);
    
    }


    function updateApplication($type,$content){
        $res=array();
        if ($this->ifExistApplication($type,$content['confKey'])){
            $res=$this->replaceApplication($type,json_encode($content));
        }else{
            $res=$this->addApplication($type,json_encode($content));
        }
        if ( preg_match("/^4..$/",$res['code'])){
            msg_dialog::display(_($type." application error"), sprintf(_("An error occurs during updating application. <br> %s <br> Detailed error : <br> %s"),$res['msg'],$res['data']['error']),ERROR_DIALOG);
        }else{
            msg_dialog::display(_($type." application information"), sprintf(_("Application has been successfully updated<br>Message : %s"),$res['msg']),INFO_DIALOG);
        }
    }

    function deleteApplication($type,$appConfKey){
        $res=array();
        if ($this->ifExistApplication($type,$appConfKey)){
            $delfct="delete".ucfirst(strtolower($type))."App";
            $handle=new accessRemoteLemonLDAPNG($this->serviceConfig);
            $res=$handle->$delfct($appConfKey);
        }
        if ( preg_match("/^4..$/",$res['code'])){
            msg_dialog::display(_($type." application error"), sprintf(_("An error occurs during updating application. <br> %s <br> Detailed error : <br> %s"),$res['msg'],$res['data']['error']),ERROR_DIALOG);
        }else{
            msg_dialog::display(_($type." application information"), sprintf(_("Application has been successfully deleted<br>Message : %s"),$res['msg']),INFO_DIALOG);
        }
    }



    // Transforme Pivot Format to remote LL:NG

    function configPivotToRemote($pivotConfig,$availableAttributes)
    {
        // get MApping for current Access Server
        $mappingConfig=$this->getRemoteAccessServerMapping($pivotConfig['protocol']);
        $extraClaims=array();
        
        // common
        $remoteConfig=array(
            'confKey'=> $pivotConfig['appname'],
            'macros' => $pivotConfig['macros'],
        );
        

        // dedicated part of SAML Protocol
        if ($pivotConfig['protocol'] == "SAML"){
            $remoteConfig['metadata']=$pivotConfig["settings"]['metadata'];

            // attributes
            foreach($pivotConfig['attributes'] as $k => $v){
                $exportedAttributes[$k]=array(
                        "mandatory" => strtolower($v['mandatory']),
                        "name" => $availableAttributes[$k]['fdAccessAttributeOID'],
                        "friendlyName" => $availableAttributes[$k]['fdAccessAttributeFriendlyName'],
                        "format" => $v['format']                        
                );
            }
            $remoteConfig['exportedAttributes']=$exportedAttributes;
            
        }
        // dedicated part for CAS protocol
        if ($pivotConfig['protocol'] == "CAS"){
            $remoteConfig['options']['service'] = $pivotConfig['settings']['service'];
             // attributes
             foreach($pivotConfig['attributes'] as $k => $v){
                $remoteConfig['exportedVars'][$k]=$v['remote_variable'];
             }

        }
        // dedicated part for OIDC protocol
        if ($pivotConfig['protocol'] == "OIDC"){
            $remoteConfig['clientId']=$pivotConfig['settings']['clientId'];
            $remoteConfig['options']['clientId']=$pivotConfig['settings']['clientId'];
            $remoteConfig['options']['clientSecret']=$pivotConfig['settings']['clientSecret'];
            $remoteConfig['redirectUris']=$pivotConfig['settings']['service'];
            $remoteConfig['options']['redirectUris']=$pivotConfig['settings']['service'];

            //scope
            foreach( $pivotConfig['scope'] as $scope => $claims){
                $remoteConfig['extraClaims'][$scope]=$claims['attributs'];
                if ($claims['rules']){
                    $remoteConfig['scopeRules'][$scope]=$claims['rules'];
                }
            }

            // attributes
            foreach($pivotConfig['attributes'] as $k => $v){
                $claim=$availableAttributes[$k]['fdAccessAttributeClaims'];
                $exportedVars=$k;
                if ( $pivotConfig['attributes'][$k]['format']){
                    $exportedVars=$exportedVars.";".$pivotConfig['attributes'][$k]['format'];
                }                
                if ( $pivotConfig['attributes'][$k]['table']){
                    $exportedVars=$exportedVars.";".$pivotConfig['attributes'][$k]['table'];   
                }
                // create 
                $remoteConfig['exportedVars'][$claim]=$exportedVars;
            }
        }


        // additionnal Settings
        foreach($pivotConfig['additionnalSettings'] as $k => $v){
            /*if (preg_match("|",$v)){
                $value=explode("|",$v);
            }else{
                $value=$v;
            }*/
            $remoteConfig['options'][$mappingConfig[$k]['remotesso']] = $v;
        }

        return($remoteConfig);
    }
}
