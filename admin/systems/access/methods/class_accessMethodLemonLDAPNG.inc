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
        $handle= new LemonLDAPNG($conf);
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
        $handle= new LemonLDAPNG($conf);
        return($handle->checkStatus());
    }

    function addApplication($type,$content){
        $addfct="add".ucfirst(strtolower($type))."App";
        $handle=new LemonLDAPNG($this->serviceConfig);
        $res=$handle->$addfct($content);
        return($res);
    }


    function replaceApplication($type,$content){
        $replacefct="replace".ucfirst(strtolower($type))."App";
        $handle=new LemonLDAPNG($this->serviceConfig);
        $res=$handle->$replacefct(json_decode($content)->confKey,$content);
        return($res);
    }

    function ifExistApplication($type,$appConfKey){

        $getfct="get".ucfirst(strtolower($type))."AppByConfKey";
        $handle=new LemonLDAPNG($this->serviceConfig);
        $res=$handle->$getfct($appConfKey);
        if ( $res['code'] == 200 ){
            return(true);
        }else{
            return(false);
        }
    
    }


    function checkApplication($type,$appConfKey){

        $getfct="get".ucfirst(strtolower($type))."AppByConfKey";
        $handle=new LemonLDAPNG($this->serviceConfig);
        $res=$handle->$getfct($appConfKey);
        
        if ( preg_match("/^4..$/",$res['code'])){
          msg_dialog::display(_($type." application error"), sprintf(_("An error occurs during saving application. <br> %s <br> Detailed error : <br> %s"),$res['msg'],$res['data']['error']),ERROR_DIALOG);
        }else{
          msg_dialog::display(_($type." application information"), sprintf(_("Application %s exists"),$appConfKey,),INFO_DIALOG);
        }
    
    }

    function getApplication($type,$appConfKey){

        $getfct="get".ucfirst(strtolower($type))."AppByConfKey";
        $handle=new LemonLDAPNG($this->serviceConfig);
        $res=$handle->$getfct($appConfKey);
        return($res);
        /*if ( preg_match("/^4..$/",$res['code'])){
          msg_dialog::display(_($type." application error"), sprintf(_("An error occurs during saving application. <br> %s <br> Detailed error : <br> %s"),$res['msg'],$res['data']['error']),ERROR_DIALOG);
        }else{
          msg_dialog::display(_($type." application information"), sprintf(_("Application %s exists"),$appConfKey,),INFO_DIALOG);
        }*/
    
    }


    function importApplication($type,$appConfKey){

        $getfct="get".ucfirst(strtolower($type))."AppByConfKey";
        $handle=new LemonLDAPNG($this->serviceConfig);
        $res=$handle->$getfct($appConfKey);

     
        // import scopeRule
        foreach($res['data']['scopeRules'] as $key => $value){
            $arrayScopeRule[]=$key.";".$value;
        }
        // import macros
        foreach($res['data']['macros'] as $key => $value){
            $arrayMacros[]=$key.";".$value;
        }        

        //var_dump($arrayScopeRule);
        //$imported['fdAccessApplicationScopeRules']=$arrayScopeRule;
        
        $imported['macros']=$arrayMacros;
        //return($imported);
        return($imported);
        //$format['fdAccessApplicationScopeRules']=$res['data']['scopeRules']
        //return($res['data']);
        /*if ( preg_match("/^4..$/",$res['code'])){
          msg_dialog::display(_($type." application error"), sprintf(_("An error occurs during saving application. <br> %s <br> Detailed error : <br> %s"),$res['msg'],$res['data']['error']),ERROR_DIALOG);
        }else{
          msg_dialog::display(_($type." application information"), sprintf(_("Application %s exists"),$appConfKey,),INFO_DIALOG);
        }*/
    
    }





    function updateApplication($type,$content){
        $res=array();
        if ($this->ifExistApplication($type,json_decode($content)->confKey)){
            $res=$this->replaceApplication($type,$content);
        }else{
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
            $handle=new LemonLDAPNG($this->serviceConfig);
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
        $llngapp=array('confKey'=> $appConfig['appname'], "macros" => $appConfig['macro'], "options" => array());
        $options_array=array("service","RedirectUris");
        $optionnal_config_keys=array("macros", "options");

        // set macros
        foreach($appConfig['macros'] as $macro_infos) {
            $array_macro=preg_split("/;/", $macro_infos);
            $llngapp['macros'][$array_macro[0]]=$array_macro[1];
          }
        //set options
        foreach($appConfig['options'] as $settings_infos) {
            $array_settings=preg_split("/;/", $settings_infos);
            // convert LL:NG internal value to API value ex : oidcRPMetaDataOptionRedirectUris  => redirectUris
            $sub="/^".$this->mapping[$appConfig['type']]."Options/";
            // remote options seems to be case Sensitive
            $newkey=lcfirst(preg_replace($sub,"",$array_settings[0]));
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
              
        } 

        // configuration manipulation for CAS method        
        if ( $appConfig['type']== "CAS"){
            $llng['exportedVars']=array();
            // set Service URI
            $llngapp['options']['service']=$appConfig['service'];
            // set attributes
            foreach( $appConfig['attributes'] as $key => $value){
                $llngapp['exportedVars'][$key]=$value['friendlyName'];
            }
            
               
        }
        // configuration manipulation for SAML method
        if ( $appConfig['type']== "SAML"){
            
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
            if (isset($appConfig['metadata'])){
                $llngapp['metadata']=$appConfig['metadata'];
            }
            if ($appConfig['authority']){
                $llngapp['options']['FederationEntityID']=$appConfig['authority'];
            }
               
        }

        // configuration manipulation for OIDC method        
        if ( $appConfig['type']== "OIDC"){
            $llng['exportedVars']=array();
            // add extraClaim
            $scopeArray=array();
            // move options why ????
            $llngapp['clientId'] = $llngapp['options']['clientID'];
            $llngapp['redirectUris'] = $appConfig['service'];
            unset($llngapp['options']['service']);
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
            if (!empty($scopeArray)){
                $llngapp['extraClaims']=$scopeArray;
                $llngapp['options']['extraClaims']=$scopeArray;
            }
            // /!\ en cas de replace , les valeurs sont mergée !!!  BUG ??????
            if ($appConfig['scopeRules']){
                //var_dump($appConfig['scopeRules']);
                foreach( $appConfig['scopeRules'] as $value){
                    $valueArray=preg_split("/;/",$value);
                    $scopeRulesArray[$valueArray[0]]= $valueArray[1];
                }
                $llngapp['scopeRules']=$scopeRulesArray;

            }
        
        }

        //var_dump($llngapp);
        
        //clean empty values
        foreach($optionnal_config_keys as $opt){
            if ( count($llngapp[$opt]) == 0 ){
                unset($llngapp[$opt]);
            }    
        }
        return(json_encode($llngapp));
    }

}
