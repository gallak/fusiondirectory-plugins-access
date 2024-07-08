<?php

class accessMethodKeyCloak {

    private $name="KeyCloak";
    private $code="KeyCloak";
    private $version="0.1";


    public function getMethodCode(): string{
        return($this->code);
    }

    public function getMethodName(): string{
        return($this->name);
    }

    


    // retrun array for filling select attribute
    public function getRemoteAccessServerMapping($protocol){
        $handle= new KeyCloak($conf);
        return($handle->getOptionsMapping($protocol));
    }


    function setConfig($conf) {
        $this->serviceConfig=$conf;
    }
}