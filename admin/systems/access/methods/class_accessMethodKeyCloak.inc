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

    public function getAllOptions($protocole){
        $handle= new KeyCloak($conf);
        return($handle->getOptionsMapping($protocole));
    }


        // retrun array for filling select attribute
        public function getSelectMapping($protocol){
            $selectArray=array();
            $handle= new KeyCloak($conf);
            foreach($handle->getOptionsMapping($protocol) as $key => $value){
                $selectArray[$key]=$value['label'];
            }
            return($selectArray);
        }


    function setConfig($conf) {
        $this->serviceConfig=$conf;
    }
}