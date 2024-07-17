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

    // retrun all option per protocol
    public function getRemoteAccessServerMapping($protocol){
        $handle= new accessRemoteLemonLDAPNG($conf);
        return($handle->getOptionsMapping($protocol));
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

    function getApplication($type,$appConfKey){

        $getfct="get".ucfirst(strtolower($type))."AppByConfKey";
        $handle=new accessRemoteLemonLDAPNG($this->serviceConfig);
        $res=$handle->$getfct($appConfKey);
        return($res);
    
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
        if ($this->ifExistApplication($type,json_decode($content)->confKey)){
            $res=$this->replaceApplication($type,$content);
        }else{
            var_dump($content);
            $res=$this->addApplication($type,$content);
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
            var_dump($llng_field);

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
            $llngapp['options'][$llng_field]=$array_settings[1];
              
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
