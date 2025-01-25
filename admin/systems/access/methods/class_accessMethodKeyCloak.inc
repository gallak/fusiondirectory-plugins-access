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




    function setConfig($conf) {
        $this->serviceConfig=$conf;
    }
}