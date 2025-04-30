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
                "validation" => "is_select",          
                "label" => "Authentification level"
                ),
            "fdCOMMONRule"        => array(
                    "remotesso" =>  "rule",
                    "syntax" => "It 's the description",
                    "label" => "Activation Rule",
                    "validation" => "is_string"
                ),
    
        ),
        "CAS" => array(
                "fdCASLogoutURL"        => array(
                        "remotesso" =>  "logout",
                        "syntax" => "It must an URI",
                        "label" => "Logout Url",
                        "validation" => "is_string"
                        ),
                "fdCASRule"             => array(
                        "remotesso" =>  "rule",
                        "syntax" => "It must be an perl expression",
                        "label" => "Access Rule",
                        "validation" => "is_string"
                        ),
                "fdCASUserAttribute"    => array(
                        "remotesso" =>  "userAttribute",
                        "syntax" => "It must the name of attribute wich is send as username",
                        "label" => "User Attribute",
                        "validation" => "is_string"
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
                ),
        "SAML" => array(
                "fdSAMLIDPInitiated"    => array(
                        "remotesso" =>  "enableIDPInitiatedURL",
                        "label" => "Enable IDP Initiated",
                        "syntax" => "It must be a boolean",
                        "validation" => "is_bool",
                        ),
                "fdSAMLSignSLO"         => array(
                        "remotesso" =>  "SignSLOMessage",
                        "label" => "SLO Message signature",
                        "syntax" => "It must be a boolean",
                        "validation" => "is_string",
                        ),
                "fdSAMLSignSSO"         => array(
                        "remotesso" =>  "SignSSOMessage",
                        "label" => "SSO Message signature",
                        "syntax" => "It must be a string",
                        "validation" => "is_string",
                        ),
                "fdSAMLSignatureMethod" => array(
                        "remotesso" =>  "SignatureMethod",
                        "label" => "Signature Method",
                        "syntax" => "It must be a choice",
                        "validation" => "is_string",
                        ),
                "fdSAMLFederationEntityID" => array(
                        "remotesso" =>  "FederationEntityID",
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
                "fdSAMLFederationMandatoryAttributes" => array(
                        "remotesso" =>  "FederationRequiredAttributes", 
                        "syntax" => "It must be an boolean",
                        "validation" => "is_string",
                        "label" => "Mandatory Attribtutes"
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
        //var_dump($config);
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
            //var_dump($content);
            var_dump(json_encode($content));
            $res=$this->replaceApplication($type,json_encode($content));
        }else{
            //var_dump($content);
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
