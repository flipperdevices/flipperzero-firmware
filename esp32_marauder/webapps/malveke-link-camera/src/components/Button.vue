<template>
    <button
        :disabled="disabled"
        type="button"
        class="w-full rounded-md bg-orange-600 px-3 py-2 text-[0.8125rem] font-semibold leading-5 text-white hover:bg-orange-500"
      >
        <slot />
      </button>
</template>
<script>
export default {
    props: {
        disabled: {
            type: Boolean,
            default: false
        }
    },
    methods: {
    }
}
</script>
<style lang="">
    
</style>