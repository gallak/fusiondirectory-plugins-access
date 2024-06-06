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


    //public function generateConfig(): {


    //}

/* conf CAS
   "casAppMetaDataExportedVars":{
      "appli-cas":{
         "cn":"cn",
         "mail":"mail",
         "uid":"uid"
      }
   },
   "casAppMetaDataMacros":{
      "appli-cas":{
         "macro1":"text()",
         "macro2":"text()"
      }
   },
   "casAppMetaDataOptions":{
      "appli-cas":{
         "casAppMetaDataOptionsAuthnLevel":"2",
         "casAppMetaDataOptionsComment":"Commentaire de l'appli CAS",
         "casAppMetaDataOptionsDisplayName":"Application CAS",
         "casAppMetaDataOptionsLogout":0,
         "casAppMetaDataOptionsService":"https://appli.futur.fr",
         "casAppMetaDataOptionsUserAttribute":"uid"
      }
   },

{
  "confKey": "sp-fed-prd-moodle-univ-lyon1-fr-shibboleth",
  "macros": {},
  "exportedAttributes": {
    "mail": {
      "friendlyName": "mail",
      "mandatory": "true",
      "name": "urn:oid:0.9.2342.19200300.100.1.3",
      "format": "urn:oasis:names:tc:SAML:2.0:attrname-format:uri"
    },
    "givenName": {
      "friendlyName": "givenName",
      "mandatory": "true",
      "name": "urn:oid:2.5.4.42",
      "format": "urn:oasis:names:tc:SAML:2.0:attrname-format:uri"
    },
    "eduPersonPrincipalName": {
      "friendlyName": "eduPersonPrincipalName",
      "mandatory": "true",
      "name": "urn:oid:1.3.6.1.4.1.5923.1.1.1.6",
      "format": "urn:oasis:names:tc:SAML:2.0:attrname-format:uri"
    },
    "sn": {
      "friendlyName": "sn",
      "mandatory": "true",
      "name": "urn:oid:2.5.4.4",
      "format": "urn:oasis:names:tc:SAML:2.0:attrname-format:uri"
    }
  },
  "options": {
    "checkSSOMessageSignature": "0",
    "enableIDPInitiatedURL": "0",
    "oneTimeUse": "0",
    "encryptionMode": "none",
    "signSLOMessage": "1",
    "signSSOMessage": "1",
    "sessionNotOnOrAfterTimeout": "72000",
    "checkSLOMessageSignature": "0",
    "nameIDFormat": "",
    "notOnOrAfterTimeout": "72000",
    "forceUTF8": "1"
  },
  "metadata": "<md:Entit

}



public function doLogin() {

    $options= array( "username" => $this->username, "password" => $this->password, 'base_url' => $this->server_url);
    $param = array();
    $headers = array('Accept' => 'application/json');
    $apiClient = new RestClient($options);
    $result = $apiClient->post('/login',$param,$headers);
    if ( $result->info->http_code == "200" ){
        $this->server_token = $result->decode_response()->api_key;
    }else{
        print( "Error while getting token" );
    }
  }

  private function getRemoteObjects($request){
    $param = array();
    $headers = array('Accept' => 'application/json', 'Authorization' => $this->server_token);
    $apiClient = new RestClient();
    $apiClient->set_option('headers', $headers);
    $apiClient->set_option('base_url',$this->server_url.$this->server_uri);

    $result = $apiClient->get('/object/'.$request);
    if ( $result->info->http_code == "200" ){
        return($result->decode_response());
    }else{
        print( "Error while Remote Objects" );
    }
}

*/


    public function getMethodCode(): string{
        return($this->code);
    }

    public function getMethodName(): string{
        return($this->name);
    }
    /*

    $cnxInfo = array("base_url" => $this->fdAccessServiceURL,"username" => $this->fdAccessServiceUser, "password" => $this->fdAccessServicePassword);
    $cnx->connect($cnxInfo);
    var_dump($cnx->checkStatus());*/

    /* connect to serveur */
    /*
    function connect(array $param) {
                $handle->setCnx($conf);
        return($handle->checkStatus());

            $options= array( "username" => $param("username", "password" => $param("password"), 'base_url' => $param("base_url");
            $param = array();
            $headers = array('Accept' => 'application/json');
            $this->handle = new RestClient($options);
          }
        







        $headers = array('Accept' => 'application/json');
        $apiClient = new RestClient($options);
        $result = $apiClient->post('/login',$param,$headers);
        if ( $result->info->http_code == "200" ){
            $this->server_token = $result->decode_response()->api_key;
        }else{
            print( "Error while getting token" );
        }
        print_r("Connect application\n");
    }*/
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
        $llngapp=array('confKey'=> $appConfig['appname'], "exportedVars" => array(), "macros" => array(), "options" => array());
        $options_array=array("service","RedirectUris");

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
            // set Service URI
            $llngapp['options']['service']=$appConfig['service'];
            // set attributes
            foreach( $appConfig['attributes'] as $key => $value){
                $llngapp['exportedVars'][$key]=$value['friendlyName'];
            }
            
               
        }
        // configuration manipulation for SAML method
        if ( $appConfig['type']== "SAML"){
            // set attributes
            foreach( $appConfig['attributes'] as $key => $value){
                $llngapp['exportedVars'][$key]['friendlyName']=$value['friendlyName'];
                $llngapp['exportedVars'][$key]['mandatory']=$value['mandatory'];
                $llngapp['exportedVars'][$key]['name']=$value['name'];
                
                if ( $value['format'] == "URI" ){
                    $llngapp['exportedVars'][$key]['format']="urn:oasis:names:tc:SAML:2.0:attrname-format:uri";    
                }

            }

            //set metadata
            $llngapp['metadata']=$appConfig['metadata'];
               
        }

        // configuration manipulation for OIDC method        
        if ( $appConfig['type']== "OIDC"){

            // add extraClaim
            $scopeArray=array();
            
            // move options why ????
            $llngapp['clientId'] = $llngapp['options']['clientID'];
            $llngapp['redirectUris'] = $appConfig['service'];

            //unset($llngapp['options']['clientID']);
            // FIXME WORTEKS : RedirectUris a 2 endroits ?? BUG????
            //unset($llngapp['options']['RedirectUris']);
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

        var_dump($llngapp);
        //clean empty values
        //TODO
             
        return(json_encode($llngapp));
    }

}
