$(document).ready(function() {
    $("#ajaxForm").ajaxComplete(function(event, request, settings) {
        Drupal.attachBehaviors();
    });
});