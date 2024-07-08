<?php

class accessRemoteLemonLDAPNG extends accessRemoteCommon {

    private $cnx=array();
    private $uri="/api/v1";
    private $headers = array('Accept' => 'application/json', 'Content-Type' => 'application/json');
    private $param = array();

    private $currentMapping=array();

    /* local mapping 
      - remotesso : name of LL::NG variable
      - validation : rule for validate entered data
      - label : text shown in drop list    
    */
    private $mappingLLNG =array(
        "COMMON" => array(),
        "CAS" => array(
                "fdCASAuthLevel"        => array("remotesso" =>  "casAppMetaDataOptionsAuthnLevel","syntax" => "It must an integer corresponding to auth Level see : https://lemonldap-ng.org"),
                "fdCASLogoutURL"        => array("remotesso" =>  "casAppMetaDataOptionsLogout","syntax" => "It must an URI"),
                "fdCASRule"             => array("remotesso" =>  "casAppMetaDataOptionsRule"),
                "fdCASURLService"       => array("remotesso" =>  "casAppMetaDataOptionsService"),
                "fdCASUserAttribute"    => array("remotesso" =>  "casAppMetaDataOptionsUserAttribute"),
                ),
        "OIDC" => array(
                "fdOIDCClientID"        => array("remotesso" =>  "oidcRPMetaDataOptionsClientID","syntax" => "It must an string see : https://lemonldap-ng.org"),
                "fdOIDCClientSecret"    => array("remotesso" =>  "oidcRPMetaDataOptionsClientSecret"),
                "fdOIDCLogoutType"      => array("remotesso" =>  "oidcRPMetaDataOptionsLogoutType"),
                "fdOIDCRedirectURIs"    => array("remotesso" =>  "oidcRPMetaDataOptionsRedirectUris"),
                ),
        "SAML" => array(
                "fdSAMLIDPInitiated"    => array("remotesso" =>  "samlSPMetaDataOptionsEnableIDPInitiatedURL"),
                "fdSAMLSignSLO"         => array("remotesso" =>  "samlSPMetaDataOptionsSignSLOMessage"),
                "fdSAMLSignSSO"         => array("remotesso" =>  "samlSPMetaDataOptionsSignSSOMessage"),
                "fdSAMLSignatureMethod" => array("remotesso" =>  "samlSPMetaDataOptionsSignatureMethod"),
        ),

    );



    public function getOptionsMapping($protocol){
        $this->updateMappingFD($this->mappingLLNG);
        return($this->mappingFD[$protocol]);
    }


    public function __construct($cnx) {
        $this->cnx = $cnx;
        
    }

    public function checkStatus(){
            $handle = new RestClient($this->cnx);
            $result = $handle->get($this->uri."/status",$this->param,$this->headers);
	    return($this->getResult($result));
	}

   // delete application
   // $param is an associative array
   // $app is a string
   public function deleteCasApp($confKey) {
        $handle = new RestClient($this->cnx);
        $result = $handle->delete($this->uri."/providers/cas/app/".$confKey,[],$this->headers);
        return($this->getResult($result));
        }

   public function deleteSamlApp($confKey) {
        $handle = new RestClient($this->cnx);
        $result = $handle->delete($this->uri."/providers/saml/sp/".$confKey,[],$this->headers);
        return($this->getResult($result));
        }

   public function deleteOidcApp($confKey) {
        $handle = new RestClient($this->cnx);
        $result = $handle->delete($this->uri."/providers/oidc/rp/".$confKey,[],$this->headers);
        return($this->getResult($result));
        }


   // replace application 
   // $param is an associative array
   // $app is a string
   public function replaceCasApp($confKey, $param) {
        $handle = new RestClient($this->cnx);
        $req_uri=$this->uri."/providers/cas/app/".$confKey;
        $result = $handle->put($req_uri,$param,$this->headers);
        return($this->getResult($result));
        }

   public function replaceSamlApp($confKey, $param) {
        $handle = new RestClient($this->cnx);
        $req_uri=$this->uri."/providers/saml/sp/".$confKey;
        $result = $handle->put($req_uri,$param,$this->headers);
        return($this->getResult($result));
        }

   public function replaceOidcApp($confKey, $param) {
        $handle = new RestClient($this->cnx);
        $req_uri=$this->uri."/providers/oidc/rp/".$confKey;
        $result = $handle->put($req_uri,$param,$this->headers);
        return($this->getResult($result));
        }


   // add application 
   // $param is an associative array
   public function addCasApp($param) {
        $handle = new RestClient($this->cnx);
        $req_uri=$this->uri."/providers/cas/app";
	$result = $handle->post($req_uri,$param,$this->headers);
	return($this->getResult($result));
	}


   public function addSamlApp($param) {
        $handle = new RestClient($this->cnx);
        $req_uri=$this->uri."/providers/saml/sp";
        $result = $handle->post($req_uri,$param,$this->headers);
        return($this->getResult($result));
        }

   public function addOidcApp($param) {
        $handle = new RestClient($this->cnx);
        $req_uri=$this->uri."/providers/oidc/rp";
        $result = $handle->post($req_uri,$param,$this->headers);
        return($this->getResult($result));
        }

    // get application by Id
    public function getCasAppByConfKey(string $conf){
            $handle = new RestClient($this->cnx);
            $result = $handle->get($this->uri."/providers/cas/app/".$conf,[],$this->headers);
	    return($this->getResult($result));
	}

    public function getSamlAppByConfKey(string $conf){
            $handle = new RestClient($this->cnx);
            $result = $handle->get($this->uri."/providers/saml/sp/".$conf,[],$this->headers);
            return($this->getResult($result));
        }

    public function getOidcAppByConfKey(string $conf){
            $handle = new RestClient($this->cnx);
            $result = $handle->get($this->uri."/providers/oidc/rp/".$conf,[],$this->headers);
            return($this->getResult($result));
        }

    // get application by URL
    public function getCasAppByService(string $service){
            $handle = new RestClient($this->cnx);
            $result = $handle->get($this->uri."/providers/cas/app/findByServiceUrl", array("serviceUrl" => $service), $this->headers);
            return($this->getResult($result));
        }

    public function getSamlAppByService(string $service){
            $handle = new RestClient($this->cnx);
            $result = $handle->get($this->uri."/providers/saml/sp/findByEntityId", array("entityId" => $service), $this->headers);
            return($this->getResult($result));
        }

    public function getOidcAppByService(string $service){
            $handle = new RestClient($this->cnx);
            $result = $handle->get($this->uri."/providers/oidc/rp/findByClientId", array("clientId" => $service), $this->headers);
            return($this->getResult($result));
        }

    // search application by confKey
    public function searchCasAppByConfKey(string $pattern){
            $handle = new RestClient($this->cnx);
            $result = $handle->get($this->uri."/providers/cas/app/findByConfKey", array("pattern" => $pattern), $this->headers);
            return($this->getResult($result));
        }

    public function searchSamlAppByConfKey(string $pattern){
            $handle = new RestClient($this->cnx);
            $result = $handle->get($this->uri."/providers/saml/sp/findByConfKey", array("pattern" => $pattern), $this->headers);
            return($this->getResult($result));
        }

    public function searchOidcAppByConfKey(string $pattern){
            $handle = new RestClient($this->cnx);
            $result = $handle->get($this->uri."/providers/oidc/rp/findByConfKey", array("pattern" => $pattern), $this->headers);
            return($this->getResult($result));
        }



  public function getResult($response) {

	$msg="";
	$data = array();
        $code = $response->info->http_code;
	switch ($code) {
	case "204":
		$msg = "Sucessful modification";
		break;
        case "400":
                $msg = "an error was encountered when processing the request";
                $data  =json_decode(json_encode($response->decode_response()),TRUE);
                break;
        case "404":
                $msg = "The specified ressource was not found";
                break;
        case "409":
                $msg = "The specified object could not be created because its configuration key, client_id or entityID already exists";
                break;
        case "200":
                $msg = "The specified ressource has been found";
		$data = json_decode(json_encode($response->decode_response()),TRUE);
                break;
        case "201":
                $msg = "Successful creation";
                break;

        case "401":
                $msg = "Login Password Incorrect";
                break;
	}

	return(array('code' => $code, 'msg' => $msg, 'data' => $data));

  }


}
