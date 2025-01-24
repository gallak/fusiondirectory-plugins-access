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
        "COMMON" => array(),
        "CAS" => array(
                "fdCASAuthLevel"        => array(
                        "remotesso" =>  "casAppMetaDataOptionsAuthnLevel",
                        "syntax" => "It must an integer corresponding to auth Level see : https://lemonldap-ng.org",
                        "range" => array("-4","5")
                        ),
                "fdCASLogoutURL"        => array(
                        "remotesso" =>  "casAppMetaDataOptionsLogout",
                        "syntax" => "It must an URI"
                        ),
                "fdCASRule"             => array(
                        "remotesso" =>  "casAppMetaDataOptionsRule"
                        ),
                "fdCASUserAttribute"    => array(
                        "remotesso" =>  "casAppMetaDataOptionsUserAttribute"
                        ),
                ),
        "OIDC" => array(
                "fdOIDCLogoutType"      => array(
                        "remotesso" =>  "oidcRPMetaDataOptionsLogoutType",
                        "label"         => "Type of logout",
                        "validation" => "is_string"
                        ),
                "fdOIDCAuthLevel"        => array(
                        "remotesso"=>  "oidcRPMetaDataOptionsAuthnLevel",
                        "syntax" => "It must an integer corresponding to auth Level see : https://lemonldap-ng.org",
                        "range" => array("-3","5"),
                        "validation" => "is_select",          
                        "label" => "Authentification level"
                        ),
                "fdOIDCServiceEncAlgo"  => array(
                        "remotesso" =>  "oidcServiceEncAlgorithmEnc", 
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
                ),
        "SAML" => array(
                "fdSAMLIDPInitiated"    => array(
                        "remotesso" =>  "samlSPMetaDataOptionsEnableIDPInitiatedURL"
                        ),
                "fdSAMLSignSLO"         => array(
                        "remotesso" =>  "samlSPMetaDataOptionsSignSLOMessage"
                        ),
                "fdSAMLSignSSO"         => array(
                        "remotesso" =>  "samlSPMetaDataOptionsSignSSOMessage"
                        ),
                "fdSAMLSignatureMethod" => array(
                        "remotesso" =>  "samlSPMetaDataOptionsSignatureMethod"
                        ),
                "fdSAMLFederationEntityID" => array(
                        "remotesso" =>  "samlSPMetaDataOptionsFederationEntityID",
                        "syntax" => "It must be an URL",
                        'validation'=> 'is_string'
                        ),
                "fdSAMLFederationOptionalAttributes" => array(
                        "remotesso" =>  "samlSPMetaDataOptionsFederationOptionalAttributes",
                        "syntax" => "It must be an boolean",
                        'validation'=> 'is_string',
                        'label'=> 'Optionnal Attribtutes'),
                "fdSAMLFederationMandatoryAttributes" => array(
                        "remotesso" =>  "samlSPMetaDataOptionsFederationRequiredAttributes", 
                        "syntax" => "It must be an boolean",
                        'validation'=> 'is_string',
                        'label'=> 'Mandatory Attribtutes'
                        ),
        ),

    );




    // retrun all option per protocol
    public function getRemoteAccessServerMapping($protocol){
        //$handle= new accessRemoteLemonLDAPNG($this->serviceConfig);
        //return($handle->getOptionsMapping($protocol));
        return($this->mappingLLNG[$protocol]);
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
                foreach ($config[$k] as $subk => $subv){
                    if (empty($subv)){
                        unset($config[$k][$subk]);
                    }
                }
            }
        }
        return($config);
    }

    // Transform remote LL::NG response to Pivot Format
    function configRemoteToPivot($type,$remoteConfig,$availableAttributes){

        $cleanConfig=$this->cleanRemoteConfig($remoteConfig);
        //var_dump("Version Clean Config");
        //var_dump($cleanConfig);
        $remotePivot=array();
        // check Type of authentification
        // manage attributes


        foreach ($cleanConfig['exportedVars'] as $k => $v){
            $remotePivot['attributes'][$k]['dn']=$availableAttributes[$k]['dn'];
            if( empty($remotePivot['attributes'][$k]['friendlyName'])){
                $remotePivot['attributes'][$k]['friendlyName']=$availableAttributes[$k]['fdAccessAttributeAlias'];
            }
            if( empty($remotePivot['attributes'][$k]['name'])){
                $remotePivot['attributes'][$k]['name']=$availableAttributes[$k]['fdAccessAttributeOID'];
            }  
        }

/*
        global $config;
        $ldap = $config->get_ldap_link();
        // FIXME should be more optmized
        $ldap->cd("dc=demo,dc=fusion");
        foreach ($cleanConfig['exportedVars'] as $k => $v){
            $ldap->search('(&(objectClass=fdAccessAttributes)(cn='.$k.'))', ['dn','fdAccessAttributeOID','fdAccessAttributeAlias']);
            while( $attr_data = $ldap->fetch()) {
                $remotePivot['attributes'][$k]['dn']=$attr_data['dn'];
                if( empty($remotePivot['attributes'][$k]['friendlyName'])){
                    $remotePivot['attributes'][$k]['friendlyName']=$attr_data['fdAccessAttributeAlias'][0];
                }
                if( empty($remotePivot['attributes'][$k]['name'])){
                    $remotePivot['attributes'][$k]['name']=$attr_data['fdAccessAttributeOID'][0];
                }        
            }
        }
*/






        $remotePivot['appname']=$cleanConfig['confKey'];
        $remotePivot['description']=$cleanConfig['options']['comment'];
        $remotePivot['macros']=$cleanConfig['macros'];
        $remotePivot['protocol']=$type;


        // I'm a OIDC config
        
        if ($type == "OIDC"){
            // main setttings
            $remotePivot['settings']['service']=$cleanConfig['redirectUris'];
            $remotePivot['settings']['clientId']=$cleanConfig['clientId'];
            $remotePivot['settings']['clientSecret']=$cleanConfig['options']['clientSecret'];
            // format and table of OIDC attribute
            foreach ($cleanConfig['exportedVars'] as $k => $v){
                $attributes_array=explode(";",$v);
                $remotePivot['attributes'][$k]['format']=$attributes_array[1];
                $remotePivot['attributes'][$k]['table']=$attributes_array[2];
            }
            foreach ($cleanConfig['extraClaims'] as $k => $v){
                $claims=explode(" ",$v);
                foreach ($claims as $claim){
                    $remotePivot['attributes'][$claim]['scope']=$k;
                }
            }
        }


        foreach ($this->getRemoteAccessServerMapping($type) as $fdAttr => $fdValue){
            foreach ($cleanConfig['options'] as $remoteAttr => $remoteValue){
                if ($fdValue['remotesso'] == $remoteAttr ){
                    $remotePivot['additionnalSettings'][$fdAttr] =$remoteValue;
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
        //return($imported);
        return($imported);
    
    }





    function updateApplication($type,$content){
        $res=array();
        //if ($this->ifExistApplication($type,json_decode($content)->confKey)){
        if ($this->ifExistApplication($type,$content['confKey'])){
            var_dump($content);
            var_dump(json_encode($content));
            $res=$this->replaceApplication($type,json_encode($content));
        }else{
            var_dump($content);
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

    function configPivotToRemote($pivotConfig)
    {
        // get MApping for current Access Server
        $mappingConfig=$this->getRemoteAccessServerMapping($pivotConfig['protocol']);
        //var_dump($mappingConfig);
        $extraClaims=array();
        
        // common
        $remoteConfig=array(
            'confKey'=> $pivotConfig['appname'],
            'macros' => $pivotConfig['macro'],
            'options' => array('comment' => $pivotConfig['description'])
        );
        
        $remoteConfig['redirectUris']=$pivotConfig['settings']['service'];
        $remoteConfig['options']['redirectUris']=$pivotConfig['settings']['service'];

        if ($pivotConfig['protocol'] == "OIDC"){
            $remoteConfig['clientId']=$pivotConfig['settings']['clientId'];
            $remoteConfig['options']['clientId']=$pivotConfig['settings']['clientId'];
            $remoteConfig['options']['clientSecret']=$pivotConfig['settings']['clientSecret'];
        }

        // attributes

        foreach($pivotConfig['attributes'] as $k => $v){
            $exportedVars=$pivotConfig['attributes'][$k]['friendlyName'];
            if ( $pivotConfig['attributes'][$k]['format']){
                $exportedVars=$exportedVars.";".$pivotConfig['attributes'][$k]['format'];
            }

            
            if ( $pivotConfig['attributes'][$k]['table']){
                $exportedVars=$exportedVars.";".$pivotConfig['attributes'][$k]['table'];   
            }
            // create 
            $remoteConfig['exportedVars'][$k]=$exportedVars;
            if ( $pivotConfig['attributes'][$k]['scope']){
                   $extraClaims[$pivotConfig['attributes'][$k]['scope']][]=$k;
            } 
        }
        foreach( $extraClaims as $scope => $claims){
            $remoteConfig['extraClaims'][$scope]=implode(" ",$claims);
        }
        // options
        return($remoteConfig);
    }


//on s'insipre d'en dessous


    function generateConfig(array $appConfig) {
        
        $service=array();
        // bug exportVars for cas and OIDC axportedAttributes for saml put it in dedicated portion code
        $llngapp=array('confKey'=> $appConfig['appname'], "macros" => $appConfig['macro']); //, "options" => array());
        $options_array=array("service","RedirectUris");
        $optionnal_config_keys=array("macros", "options");

        $mappingConfig=$this->getRemoteAccessServerMapping($appConfig['auth']['type']);

        // set macros
        foreach($appConfig['macros'] as $macro_infos) {
            $array_macro=preg_split("/;/", $macro_infos);
            $llngapp['macros'][$array_macro[0]]=$array_macro[1];
          }
        //set options
        foreach($appConfig['settings'] as $settings_infos) {
            $array_settings=preg_split("/;/", $settings_infos);
            // convert LL:NG internal value to API value ex : oidcRPMetaDataOptionRedirectUris  => redirectUris
            $sub="/^".$this->mapping[$appConfig['auth']['type']]."Options/";
            // remote options seems to be case Sensitive

            $llng_field=$mappingConfig[$array_settings[0]]['remotesso'];

            $newkey=lcfirst(preg_replace($sub,"",$llng_field));
            // take care of multivaluated variables
            
            if ( in_array($newkey,  $options_array )){
                if ( $llngapp['options'][$newkey] == null ){
                    $llngapp['options'][$newkey]=array($array_settings[1]);   
                }else{
                    array_push($llngapp['options'][$newkey],$array_settings[1]);
                }
            }else{
                $llngapp['options'][$newkey]=$array_settings[1];
            }
            //$llngapp['options'][$llng_field]=$array_settings[1];
              
        } 

        // set description

        $llngapp['options']['comment'] = $appConfig['description'];

        // configuration manipulation for CAS method        
        if ( $appConfig['auth']['type']== "CAS"){
            $llng['exportedVars']=array();
            // set Service URI
            $llngapp['options']['service']=$appConfig['auth']['service'];
            // set attributes
            foreach( $appConfig['attributes'] as $key => $value){
                $llngapp['exportedVars'][$key]=$value['friendlyName'];
            }
            
               
        }
        // configuration manipulation for SAML method
        if ( $appConfig['auth']['type']== "SAML"){
            
            $llng['exportedAttributes']=array();
            // set attributes
            foreach( $appConfig['attributes'] as $key => $value){
                $llngapp['exportedAttributes'][$key]['friendlyName']=$value['friendlyName'];
                $llngapp['exportedAttributes'][$key]['mandatory']=$value['mandatory'];
                $llngapp['exportedAttributes'][$key]['name']=$value['name'];
                
                if ( $value['format'] == "URI" ){
                    $llngapp['exportedAttributes'][$key]['format']="urn:oasis:names:tc:SAML:2.0:attrname-format:uri";    
                }

            }

            //set metadata
            if (isset($appConfig['auth']['metadata'])){
                $llngapp['metadata']=$appConfig['auth']['metadata'];
            }
               
        }

        // configuration manipulation for OIDC method        
        if ( $appConfig['auth']['type']== "OIDC"){
            $llng['exportedVars']=array();
            // add extraClaim
            $scopeArray=array();

            // Set dedicated settings redirectUris / clientId
            $llngapp['redirectUris'] = $appConfig['auth']['service'];
            $llngapp['clientId'] = $appConfig['auth']['clientId'];
            // set settings clientSecret
            if (! empty($appConfig['auth']['clientSecret'])){
                $llngapp['options']['clientSecret']=$appConfig['auth']['clientSecret'];
            }

            //unset($llngapp['options']['service']);
            // add attribute
            foreach( $appConfig['attributes'] as $key => $value){
                //var_dump($value);
                $llngapp['exportedVars'][$key]=$value['friendlyName'];
                if (! empty($value['scope'])){
                    if ($scopeArray[$value['scope']] == null ){
                        $scopeArray[$value['scope']]=$key;
                    }else{
                        $scopeArray[$value['scope']]=$scopeArray[$value['scope']]." ".$key;
                    }
                    
                }
            }
            // add extraClaims
            if (!empty($scopeArray)){
                $llngapp['extraClaims']=$scopeArray;
                //$llngapp['options']['extraClaims']=$scopeArray;
            }
            // /!\ en cas de replace , les valeurs sont mergée !!!  BUG ??????
            // add scopeRules
            if ($appConfig['scopeRules']){
                //var_dump($appConfig['scopeRules']);
                foreach( $appConfig['scopeRules'] as $value){
                    $valueArray=preg_split("/;/",$value);
                    $scopeRulesArray[$valueArray[0]]= $valueArray[1];
                }
                $llngapp['scopeRules']=$scopeRulesArray;

            }
        
        }

        
        //clean empty values
        foreach($optionnal_config_keys as $opt){
            if ( count($llngapp[$opt]) == 0 ){
                unset($llngapp[$opt]);
            }    
        }
        return(json_encode($llngapp));
    }

}
